#pragma once
#include <queue>
#include <algorithm>
#include <stack>
#include "BoxCollider.h"

namespace RipeGrain
{
    class BinaryBoundingBoxTree
    {
    public:
        struct Node
        {
            friend BinaryBoundingBoxTree;
        private:
            Node* parent;
            Node* left_child;
            Node* right_child;
            BoxCollider bounding_box;
        public:
            Node() : parent(nullptr), left_child(nullptr), right_child(nullptr) {}
            Node(BoxCollider collider) : bounding_box(collider), parent(nullptr), left_child(nullptr), right_child(nullptr) {}
            ~Node()
            {
                if (left_child)
                    delete left_child;
                if (right_child)
                    delete right_child;
            }
        public:
            BoxCollider GetCollider() const
            {
                return bounding_box;
            }
        public:
            bool IsLeafNode() const
            {
                return (left_child == nullptr) && (right_child == nullptr);
            }
        };
    private:
        Node* root = nullptr;
    public:
        ~BinaryBoundingBoxTree()
        {
            delete root;
        }
    private:
        static BoxCollider makeUnion(BoxCollider collider1, BoxCollider collider2)
        {
            return BoxCollider
            {

                (std::min)(collider1.GetLeft() , collider2.GetLeft()),
                (std::max)(collider1.GetRight() , collider2.GetRight()),
                (std::min)(collider1.GetTop() , collider2.GetTop()),
                (std::max)(collider1.GetBottom() , collider2.GetBottom())
            };
        }
        static int calculateInheritedCost(Node* node, BoxCollider collider)
        {
            return makeUnion(collider, node->bounding_box).GetArea() - node->bounding_box.GetArea();
        }
        static int calculateTotalAncestorCost(Node* node, BoxCollider collider)
        {
            int cost = 0;
            if (node)
            {
                while (node = node->parent)
                {
                    cost += calculateInheritedCost(node, collider);
                }
            }
            return cost;
        }
        static void refitAncestors(Node* node)
        {
            if (node)
            {
                while (node = node->parent)
                {
                    if (node->left_child && node->right_child)
                    {
                        node->bounding_box = makeUnion(node->left_child->bounding_box, node->right_child->bounding_box);
                    }
                    else if (node->left_child)
                    {
                        node->bounding_box = node->left_child->bounding_box;
                    }
                    else
                    {
                        node->bounding_box = node->right_child->bounding_box;
                    }
                }
            }
        }
        static Node* createNewParent(BoxCollider collider, Node* child)
        {
            Node* new_parent = new Node();
            child->parent = new_parent;
            new_parent->right_child = child;
            new_parent->left_child = new Node(collider);
            new_parent->left_child->parent = new_parent;
            new_parent->right_child->parent = new_parent;
            new_parent->bounding_box = makeUnion(collider, child->bounding_box);
            return new_parent;
        }
    private:
        Node* searchBestSibling(BoxCollider collider) const
        {
            struct NodeCost
            {
                Node* node;
                int cost;

                NodeCost() : node(nullptr), cost(0) {}
                NodeCost(Node* node, int cost) : node(node), cost(cost) {}

                bool operator< (const NodeCost& n) const
                {
                    return cost < n.cost;
                }
            };

            NodeCost best_node;

            std::priority_queue<NodeCost> s;
            s.emplace(root, root->bounding_box.GetArea());

            while (!s.empty())
            {
                auto node_cost = s.top();
                s.pop();

                int ancestral_cost = calculateTotalAncestorCost(node_cost.node, collider);
                int cost = makeUnion(node_cost.node->bounding_box, collider).GetArea() + ancestral_cost;
                if (best_node.node == nullptr || cost < best_node.cost)
                {
                    best_node = node_cost;
                    best_node.cost = cost;
                }

                int lower_cost = collider.GetArea() + calculateInheritedCost(node_cost.node, collider) + ancestral_cost;
                if (lower_cost < best_node.cost)
                {
                    if (node_cost.node->left_child)
                        s.emplace(node_cost.node->left_child, lower_cost);
                    if (node_cost.node->right_child)
                        s.emplace(node_cost.node->right_child, lower_cost);
                }
            }

            return best_node.node;
        }
    public:
        void Insert(BoxCollider collider)
        {
            if (root)
            {
                if (!root->IsLeafNode())
                {
                    auto n = searchBestSibling(collider);
                    auto prev_parent = n->parent;
                    auto new_parent = createNewParent(collider, n);
                    if (n == root)
                    {
                        root = new_parent;
                    }
                    else
                    {
                        if (prev_parent->left_child == n)
                        {
                            prev_parent->left_child = new_parent;
                        }
                        else
                        {
                            prev_parent->right_child = new_parent;
                        }
                        new_parent->parent = prev_parent;
                    }
                    refitAncestors(new_parent);
                }
                else
                {
                    auto new_parent = createNewParent(collider, root);
                    root = new_parent;
                }
            }
            else
            {
                root = new Node();
                root->bounding_box = collider;
            }
        }
        void RemoveNode(Node* node)
        {
            if (node == root)
            {
                delete root;
                root = nullptr;
            }
            else
            {
                auto parent = node->parent;
                auto sibling = parent->left_child == node ? parent->right_child : parent->left_child;

                // if parent is root then set the sibling as root
                if (parent == root)
                {
                    root = sibling;
                    sibling->parent = nullptr;
                }
                // else set the sibling as direct descendent of it's grand-parent
                else
                {
                    if (parent->parent->left_child == parent)
                    {
                        parent->parent->left_child = sibling;
                    }
                    else
                    {
                        parent->parent->right_child = sibling;
                    }
                    sibling->parent = parent->parent;
                }
                refitAncestors(parent);
                parent->left_child = parent->right_child = nullptr;
                delete node;
                delete parent;
            }
        }
        void Clear()
        {
            delete root;
            root = nullptr;
        }
    public:
        std::vector<std::pair<Node*, Node*>> TestOverlaps() const
        {
            std::vector<std::pair<Node*, Node*>> colliders;
            if (root && !root->IsLeafNode())
            {
                std::stack<std::pair<Node*, Node*>> s;
                s.emplace(root->left_child, root->right_child);
                while (!s.empty())
                {
                    auto [n1, n2] = s.top();
                    s.pop();

                    if (n1->bounding_box.IsCollidingWith(n2->bounding_box))
                    {
                        if (n1->IsLeafNode() && n2->IsLeafNode())
                        {
                            colliders.emplace_back(n1, n2);
                        }
                        else
                        {
                            Node* leaf_node = nullptr;
                            Node* non_leaf_node = nullptr;
                            if (n1->IsLeafNode())
                            {
                                leaf_node = n1;
                                non_leaf_node = n2;
                            }
                            else
                            {
                                leaf_node = n2;
                                non_leaf_node = n1;
                            }

                            s.emplace(leaf_node, non_leaf_node->left_child);
                            s.emplace(leaf_node, non_leaf_node->right_child);
                        }
                    }
                    else
                    {
                        if (!n1->IsLeafNode())
                            s.emplace(n1->left_child, n1->right_child);

                        if (!n2->IsLeafNode())
                            s.emplace(n2->left_child, n2->right_child);
                    }
                }
            }
            return colliders;
        }
        std::vector<Node*> TestOverlaps(BoxCollider collider) const
        {
            std::vector<Node*> colliders;
            if (root && root->bounding_box.IsCollidingWith(collider))
            {
                std::stack<Node*> s;
                s.push(root);
                while (!s.empty())
                {
                    auto node = s.top();
                    s.pop();

                    if (node->IsLeafNode())
                    {
                        colliders.push_back(node);
                        continue;
                    }

                    if (node->left_child && node->left_child->bounding_box.IsCollidingWith(collider))
                        s.push(node->left_child);
                    if (node->right_child && node->right_child->bounding_box.IsCollidingWith(collider))
                        s.push(node->right_child);
                }
            }
            return colliders;
        }
    private:
        static bool IsInside(const BoxCollider& child, const BoxCollider& parent)
        {
            return child.GetLeft() >= parent.GetLeft()
                && child.GetRight() <= parent.GetRight()
                && child.GetBottom() <= parent.GetBottom()
                && child.GetTop() >= parent.GetTop();
        }
    public:
        std::vector<Node*> CheckInvalidNodes() const
        {
            std::vector<Node*> nodes;
            int count = 0;
            if (root && !root->IsLeafNode())
            {
                std::stack<Node*> s;
                s.push(root);
                while (!s.empty())
                {
                    auto n = s.top();
                    s.pop();

                    if (n->IsLeafNode() && !IsInside(n->bounding_box , n->parent->bounding_box))
                    {
                        nodes.push_back(n);
                        continue;
                    }

                    if (n->left_child)
                        s.push(n->left_child);
                    if (n->right_child)
                        s.push(n->right_child);
                }
            }
            return nodes;
        }
    };
}