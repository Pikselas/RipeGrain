#pragma once
#include <new>
#include <map>
#include <mutex>
#include <memory>
#include <vector>
#include <typeindex>
#include <functional>

namespace RipeGrain
{
    namespace Pool
    {
        class PoolCommonType
        {
        private:
            const unsigned int byte_size = 0;
        public:
            PoolCommonType(unsigned int size) : byte_size(size) {}
		public:
			unsigned int size() const { return byte_size; }
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

        template<typename T>
        class PoolObjectRef
        {
            template <typename U>
            friend class PoolObjectRef;
        private:
            std::shared_ptr<PoolCommonType> obj_ref;
        public:
            PoolObjectRef() : obj_ref(nullptr) {}
            PoolObjectRef(std::shared_ptr<PoolCommonType> ref)
                : obj_ref(std::move(ref))
            {}
            template <typename U>
            requires CBaseClass<T,U>
            PoolObjectRef(const PoolObjectRef<U>& other) : PoolObjectRef(other.obj_ref) {}
        public:
            template <typename U>
            requires CBaseClass<T, U>
            PoolObjectRef& operator=(const PoolObjectRef<U>& other)
            {
                obj_ref = other.obj_ref;
				return *this;
            }
        public:
            T* get() const
            {
				unsigned int obj_size = obj_ref->size();
                return &reinterpret_cast<pool_obj_holder<T>*>(obj_ref.get())->obj;
            }
            T* operator->() const
            {
                return get();
            }
        public:
            explicit operator bool() const { return static_cast<bool>(obj_ref); }
        };

        template <>
        class PoolObjectRef<PoolCommonType>
        {
        private:
            std::shared_ptr<PoolCommonType> obj_ref;
        public:
            PoolObjectRef() = default;
            PoolObjectRef(std::shared_ptr<PoolCommonType> ref) : obj_ref(std::move(ref)) {}
        public:
            template <typename T>
            PoolObjectRef(const PoolObjectRef<T>& other)
                : obj_ref(other.obj_ref)
            {}
        public:
            PoolCommonType* get() const { return obj_ref.get(); }
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
                return { std::shared_ptr<PoolCommonType>
                    (
                        obj,
                        std::bind(&ObjectPool::return_to_pool_helper<T>, pool , std::placeholders::_1)
                    ) };
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