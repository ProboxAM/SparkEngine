#ifndef _PANEL_HIERARCHY_H_
#define _PANEL_HIERARCHY_H_

#include "Panel.h"

class ComponentTransform;

enum MOUSE_INPUT {LEFT_CLICK, RIGHT_CLICK};
class PanelHierarchy :
	public Panel
{
public:
	PanelHierarchy(bool a);
	~PanelHierarchy();

	void Draw();
	void DrawNode(ComponentTransform* ct);

	void SetDragAndDropTarget(ComponentTransform* target);
	void SetDragAndDropSource(ComponentTransform* target);
	void SetDragAndDropTargetCustom();

	void OnNodeRightClick();

	int node_iterator = -1;
};

#endif // !_PANEL_HIERARCHY_H_

