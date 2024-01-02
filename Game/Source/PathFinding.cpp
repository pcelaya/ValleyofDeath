#include "PathFinding.h"

#include "App.h"
#include "Map.h"
#include "PathFinding.h"

#include "Defs.h"
#include "Log.h"

PathFinding::PathFinding()
{
	name.Create("pathfinding");
	map = NULL;
	width = 0;
	height = 0;
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called befor exit
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	lastPath.Clear();
	RELEASE_ARRAY(map);

	return true;
}

// Sets up the walkability map
void PathFinding::SetNavigationMap(uint w, uint h, uchar* data)
{
	width = w;
	height = h;

	RELEASE_ARRAY(map);
	map = new uchar[width * height];
	memcpy(map, data, width * height);
}

// Utility: return true if pos is inside the map boundaries
bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.y >= 0 && pos.x <= (int)width && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar walkID = GetTileAt(pos);
	bool isWalkable = (walkID != INVALID_WALK_CODE && walkID > 0);

	return isWalkable;
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}


// To request all tiles involved in the last generated path
const DynArray<iPoint>* PathFinding::GetLastPath() const
{
	return &lastPath;
}

// Delete last path used
void PathFinding::ClearLastPath()
{
	lastPath.Clear();
}

// Main function to request a path from A to B
int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	int ret = -1;

	bool debug = IsWalkable(origin);
	bool debug2 = IsWalkable(destination);

	if (!IsWalkable(origin) || !IsWalkable(destination))
		return ret;
	
	PathList frontier;
	PathList visited;

	frontier.list.Add(PathNode(0, 0, origin, NULL));

	// Iterate while there are nodes in the frontier list
	while (frontier.list.Count() > 0)
	{
		ListItem<PathNode>* low = frontier.GetNodeLowestScore();
		ListItem<PathNode>* node = new ListItem<PathNode>(*low);

		visited.list.Add(low->data);
		frontier.list.Del(low);


		if (node->data.pos == destination)
		{
			lastPath.Clear();
			const PathNode* pathNode = &node->data;

			while (pathNode)
			{
				lastPath.PushBack(pathNode->pos);
				pathNode = pathNode->parent;
			}

			lastPath.Flip();
			ret = lastPath.Count();
			break;
		}


		PathList adjacent;
		node->data.FindWalkableAdjacents(adjacent);

		ListItem<PathNode>* neighbour = adjacent.list.start;
		while (neighbour != NULL)
		{
			if (visited.Find(neighbour->data.pos) == NULL)
			{
				visited.list.Add(neighbour->data);

				if (frontier.Find(neighbour->data.pos) == NULL)
				{
					neighbour->data.CalculateF(destination);
					frontier.list.Add(neighbour->data);
				}
				else
				{
					if (frontier.Find(neighbour->data.pos)->data.g > neighbour->data.g + 1)
					{
						frontier.Find(neighbour->data.pos)->data.parent = neighbour->data.parent;
						frontier.Find(neighbour->data.pos)->data.CalculateF(destination);
					}
				}
			}

				neighbour = neighbour->next;
		}
	}

	return ret;
}



// PathList
ListItem<PathNode>* PathList::Find(const iPoint& point) const
{
	ListItem<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return item;
}

ListItem<PathNode>* PathList::GetNodeLowestScore() const
{
	ListItem<PathNode>* ret = NULL;
	int min = 65535;

	ListItem<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}

	return ret;
}



// PathNode 
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL) {}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent) {}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent) {}

uint PathNode::FindWalkableAdjacents(PathList& listToFill) const
{
	iPoint tile;
	uint before = listToFill.list.Count();

	// top
	tile.Create(pos.x, pos.y + 1);
	if (app->map->pathfinding->IsWalkable(tile)) 
		listToFill.list.Add(PathNode(-1, -1, tile, this));

	// bottom
	tile.Create(pos.x, pos.y - 1);
	if (app->map->pathfinding->IsWalkable(tile)) 
		listToFill.list.Add(PathNode(-1, -1, tile, this));

	// left
	tile.Create(pos.x + 1, pos.y);
	if (app->map->pathfinding->IsWalkable(tile)) 
		listToFill.list.Add(PathNode(-1, -1, tile, this));

	// right
	tile.Create(pos.x - 1, pos.y);
	if (app->map->pathfinding->IsWalkable(tile)) 
		listToFill.list.Add(PathNode(-1, -1, tile, this));

	return listToFill.list.Count();
}

int PathNode::Score() const
{
	return g + h;
}

int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}
