#include "SceneNode.hpp"

TRE_NS_START

SceneNode::SceneNode() : m_Parent(NULL)
{
}

SceneNode::SceneNode(SceneNode* parent) : m_Parent(parent)
{
}

SceneNode& SceneNode::CreateNode()
{
    SceneNode& node = m_Childrens.EmplaceBack(this);
    return node;
}

void SceneNode::SetParent(SceneNode* parent)
{
    m_Parent = parent;
}

TRE_NS_END