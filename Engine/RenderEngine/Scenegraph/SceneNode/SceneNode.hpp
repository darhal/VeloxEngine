#include "Core/Misc/Defines/Common.hpp"
#include "Core/DataStructure/Vector/Vector.hpp"

TRE_NS_START

class SceneNode
{
public:
    SceneNode();

    SceneNode(SceneNode* parent);

    SceneNode& CreateNode();

    void SetParent(SceneNode* parent);

private:
    SceneNode* m_Parent;
    Vector<SceneNode> m_Childrens;
};

TRE_NS_END