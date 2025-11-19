#pragma once
#include <new>
#include <map>
#include <mutex>
#include <memory>
#include <atomic>
#include <vector>
#include <typeindex>
#include <functional>

namespace RipeGrain
{
    namespace Pool
    {
        class PoolCommonType
        {
            template <typename>
            friend class PoolObjectRef;
        private:
            const unsigned int byte_size = 0;
        private:
            std::atomic_uint ref_count;
        public:
            PoolCommonType(unsigned int size) : byte_size(size) , ref_count(0) {}
		public:
			unsigned int size() const { return byte_size; }
        public:
            void addReference()
            {
                ref_count.fetch_add(1);
            }
            void removeReference()
            {
                unsigned int expected = 0;
                ref_count.compare_exchange_strong(expected , 1);
                ref_count.fetch_sub(1);
            }
            bool isAllocationEnded()
            {
                unsigned int expected = 0;
                return ref_count.compare_exchange_strong(expected , expected);
            }
        };

        class ObjectPool;

        template <typename T>
        struct pool_obj_holder : public PoolCommonType
        {
            T obj;
            pool_obj_holder() : PoolCommonType(sizeof(T)) {}
            template <typename... Args>
            explicit pool_obj_holder(Args&&... args) 
                : 
                PoolCommonType(sizeof(T)),
                obj(std::forward<Args>(args)...) {}
        };

        template<typename T , typename U>
        concept CBaseClass = std::is_base_of_v<T, U>;

        class BasePoolObjectRef
        {
        protected:
            PoolCommonType* obj_ref;
        protected:
            std::function<void(PoolCommonType*)> free_pool_object;
        protected:
            BasePoolObjectRef() : obj_ref(nullptr) {}
            BasePoolObjectRef(PoolCommonType* ref, std::function<void(PoolCommonType*)> on_life_end)
                :
                obj_ref(ref),
                free_pool_object(on_life_end)
            {
                if (obj_ref)
                    obj_ref->addReference();
            }
            ~BasePoolObjectRef()
            {
                if (obj_ref)
                {
                    obj_ref->removeReference();
                    if (obj_ref->isAllocationEnded())
                        free_pool_object(obj_ref);
                }
            }
        protected:
            BasePoolObjectRef(const BasePoolObjectRef& other)
            {
                *this = other;
            }
        public:
            BasePoolObjectRef& operator=(const BasePoolObjectRef& other)
            {
                obj_ref = other.obj_ref;
                obj_ref->addReference();
				free_pool_object = other.free_pool_object;
                return *this;
            }
        };

        template<typename T>
		class PoolObjectRef : private BasePoolObjectRef
        {
            template <typename U>
            friend class PoolObjectRef;
        public:
            template <typename U>
            requires CBaseClass<T,U>
            PoolObjectRef(const PoolObjectRef<U>& other) 
            {
                *this = other;
            }
        public:
			PoolObjectRef(PoolCommonType* ref, std::function<void(PoolCommonType*)> on_life_end) : BasePoolObjectRef(ref, on_life_end) {}
        public:
            template <typename U>
            requires CBaseClass<T, U>
            PoolObjectRef& operator=(const PoolObjectRef<U>& other)
            {
				static_cast<BasePoolObjectRef&>(*this) = static_cast<const BasePoolObjectRef&>(other);
                return *this;
            }
        public:
            // undefined behaviour if T is a derived type rather than the type at the time of construction
            T* get() const
            {
				unsigned int obj_size = obj_ref->size();
                return &reinterpret_cast<pool_obj_holder<T>*>(obj_ref)->obj;
            }
            T* operator->() const
            {
                return get();
            }
        public:
            explicit operator bool() const { return static_cast<bool>(obj_ref); }
        };

        template <>
		class PoolObjectRef<PoolCommonType> : private BasePoolObjectRef
        {
        public:
            PoolObjectRef() = default;
			PoolObjectRef(PoolCommonType* ref, std::function<void(PoolCommonType*)> on_life_end) : BasePoolObjectRef(ref, on_life_end) {}
        public:
            template <typename T>
            PoolObjectRef(const PoolObjectRef<T>& other) 
            {
                *this = other;
			}
            public:
            template <typename T>
            PoolObjectRef& operator=(const PoolObjectRef<T>& other)
			{
				static_cast<BasePoolObjectRef&>(*this) = static_cast<const BasePoolObjectRef&>(other);
				return *this;
			}
        public:
            PoolCommonType* get() const { return obj_ref; }
            PoolCommonType* operator->() const { return get(); }
        public:
            template <typename T>
            T& as() const
            {
				auto size = obj_ref->size();
                if(size != sizeof(T))
					throw std::bad_cast();
                return reinterpret_cast<pool_obj_holder<T>*>(obj_ref.get())->obj;
            }
        public:
            explicit operator bool() const { return static_cast<bool>(obj_ref); }
        };

        template <typename T = PoolCommonType>
        using PoolObject = PoolObjectRef<T>;

        using PoolObjectAny = PoolObject<>;

        class ObjectPool : std::enable_shared_from_this<ObjectPool>
        {
            std::map<std::type_index, std::vector<std::unique_ptr<PoolCommonType>>> pool;
        private:
            template <typename T>
            static void return_to_pool_helper(ObjectPool* pool, PoolCommonType* obj)
            {
                pool->release<T>(obj);
            }
            template <typename T>
            static PoolObjectRef<T> convert_to_shared(ObjectPool* pool, PoolCommonType* obj)
            {
                return {
                        obj,
                        std::bind(&ObjectPool::return_to_pool_helper<T>, pool , std::placeholders::_1)
                       };
            }
        public:
            template <typename T, typename... Args>
            PoolObjectRef<T> acquire(Args&&... args)
            {
                std::type_index t_inf = typeid(T);
                std::unique_ptr<PoolCommonType> obj = nullptr;
                if (!pool[t_inf].empty())
                {
                    obj = std::move(pool[t_inf].back());
                    pool[t_inf].pop_back();
                    new (&reinterpret_cast<pool_obj_holder<T>*>(obj.get())->obj) T(std::forward<Args>(args)...);
                }
                else
                {
                    obj = std::unique_ptr<PoolCommonType>(new pool_obj_holder<T>{ std::forward<Args>(args)... });
                }

                return convert_to_shared<T>(this, obj.release());
            }
            template <typename T>
            void release(PoolCommonType* obj)
            {
                std::type_index t_inf = typeid(T);
                reinterpret_cast<pool_obj_holder<T>*>(obj)->obj.~T();
                pool[t_inf].emplace_back(obj);
            }
        };
    }
}