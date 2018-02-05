//some stuff for npc non-stupide navigation of obstacles

//odd task of partitioning maps
//kind of rememper how bsp's worked
//think it'd just be take a tile, find all other tiles that are in view
//good idea might be go in a cirular pattern,from some "center"
//could jankily do that by taking average of closest wall <, >, ^, v, for every cell
//go from highest distance, check things in circle, adding to senter section
//issue of marking off spaces of map
#include myMapSecionts.h
//roughly partitions into sections, next step would be finding adjecent sections
//doesn't solve issues of things like holes, where things can be seen but not walked on
//might need to change alg to look for closest non-walkable thing
//then things that could "fly" over gaps would be stupide
//could probably set that as a flag in npc, have move alg take that into account
//anyway, issue of finding adjacent sections
//first idea was storing for each sections, some list of transition-tiles
//describing from which direction a new section may be found
//also thought about storing section as a number/thing as a field either in tile or tile_pos


void makeSections(tile_map_t* map) {
  //take tiles, find avg distance, find max avg distance, traverse in cirlular pattern
  //magically know when to stop
  //remove encountered cells from list structure

  //first, shove into my list structure
  tile_dist_list_t* theList = createTile_dist_list();
  tile_dist_node_t* theNode;
  for(int rowI = 0; rowI < map->rows; rowI++) {
    for(int colI = 0; colI < map->cols; colI++) {
      tile_t tile = *getTileFromMapCord(map, colI, rowI);
      theNode = createTile_dist_node(&tile);
      prependToTile_dist_list(theList, theNode);
    }
  }
  theNode = theList->start;
  //set up a loop, to process each  tile
  while(theNode != theList->end) {
    setAvgDist(map, theNode);
    theNode = theNode->next;
  }
  //sort the list;
  mergeSort(theList->start);
  //redo all the backlinks
  redoBacklinks(theList);

  //then, go through list, take max value
  tile_dist_node_t* max = theList->end;
  int sectionCount = 1;
  while(max != NULL) {
    //positions of center tile;
    tile_pos_t* center = max->storedTile->tilePosition;
    center->section = sectionCount;
    //find out a way to trace out a cirle around node.
    //seenInLastPass, indicated a condition for loop
    //also indicates size of square to trace out
    int loopCount = 1;
    int seenInLastPass = 1;
    while(seenInLastPass) {
      seenInLastPass = checkSquares(theList,map, center, loopCount);
    }
    max = theList->end;
    sectionCount++;
  }
  

}

int checkSquares(tile_dist_list_t* theList, tile_map_t* map, tile_pos_t* center, int loopCount) {
  int foundNewTile = 0;
  tile_pos_t* tile;
  tile->posX = center->posX - loopCount;
  tile->posY = center->posY - loopCount;
  //start in top left, go inc x until  center.x - tile.x = -LC
  while( (tile->posX - center->posX) >=  loopCount) {
    foundNewTile += makeSectionsCheckTile(map,theList,  tile, center);
    tile->posX++;
  }
  //then, start incrementing y till center.y - tile.y  == -LC
  while( (tile->posY - center->posY) >=  loopCount) {
    foundNewTile += makeSectionsCheckTile(map, theList,  tile, center);
    tile->posY++;
  }
  //then, start decrementing x till center.x - tile.x  == LC
  while( (tile->posX - center->posX) <=  loopCount) {
    foundNewTile += makeSectionsCheckTile(map, theList,  tile, center);
    tile->posX--;
  }
  //then, start decrementnig y till center.y - tile.y  == LC
  while( (tile->posY - center->posY) <=  loopCount) {
    foundNewTile += makeSectionsCheckTile(map, theList,  tile, center);
    tile->posY--;
  }
  return foundNewTile;
}

int makeSectionsCheckTile(tile_map_t* map, tile_dist_list_t* theList,  tile_pos_t* tile, tile_pos_t* center) {
  tile_t* mapTile = getTileFromMapPos(map, tile);
  int found = 0;
  if(isAWall(mapTile))) {
    preMarkOffSect(center, tile);
    found = 0;
  }
  else {
    if (mapTile->section <= 0) {
      removeFromTile_dist_list(theList, mapTile
      mapTile->section = center->section;
      found = 1;
    }
  }
  return found; 
}


void preMarkOffSect(tile_pos_t* center, tile_pos_t* tile)  {
  //might need to play with this a bit. Might not even be a constant. 
  int cornerScale = 2;
  tile_pos_t* corner;
  corner->posX = cornerScale * (tile->posX - center->posX);
  corner->posY = cornerScale * (tile->posY - center->posY);
  markOffSect(tile, corner);
}

void markOffSect(tile_pos_t* c1, tile_pos_t* c2) {
  for(int i = c1->posX; i <= c2->posX; i++) {
    for(int j = c1->posY; i <= c2->posY; j++) {
      //mark stuff off
      //also still not sure on row/col order
      //had idea of marking the tilepossection the negative of center
      //so if in section 5, and marking off a sector
      //go through tiles, marking section as negative 5
      //would have to change makeSectionchecktile to address this
      //basically ignore a tile if pos > 0 or pos = -centpos
      //tile_t tile = getTileFromCord(map,j,i);
    }
  }
}

void redoBacklinks(tile_dist_list_t * theList) {
  if (theList != NULL) {
    tile_dist_node_t* node = theList->start;
    tile_dist_node_t* prevNode = NULL;
    while(node != NULL) {
      node->prev = prevNode;
      prevNode = node;
      node = node->next;
    }
    theList->end = prevNode;
    theList->end->next = NULL;
  }
}

//got this code more or less from https://www.geeksforgeeks.org/merge-sort-for-linked-list/
//so if it's not working, check that in future
void mergeSort(tile_dist_node_t* startRef) {
  if (startRef == NULL || startRef->next == NULL) {
    //nothind do do
  }
  else {
    //divide list into two
    tile_dist_node_t* start, * firstHalf, * secondHalf;
    firstHalf = NULL;
    secondHalf = NULL;
    start = startRef;
    splitList(start, firstHalf, secondHalf);
    //sort two halves
    mergeSort(firstHalf);
    mergeSort(secondHalf);
    startRef = merge(firstHalf, secondHalf);    
  }
  
}

tile_dist_node_t* merge(tile_dist_node_t* first, tile_dist_node_t* second) {
  if (first == NULL) {
    return second;
  }
  else if (second == NULL) {
    return first;
  }
  else {
    tile_dist_node_t* result = NULL;
    if (compare(first, second) < 0) {
      result = first;
      result->next = merge(first->next, second);
    }
    else {
      result = second;
      result->next = merge(first, second->next);
    }
    
    return result; 
  }
}

int compare(tile_dist_node_t* first, tile_dist_node_t* second) {
  //convention, positive if first > second, negative if first < second, zero if equal
  return first->dVal - second->dVal;
}

void splitList(tile_dist_node_t* start,tile_dist_node_t* firstHalf, tile_dist_node_t* secondHalf) {
  tile_dist_node_t *  slow, * fast;
  if (start == NULL || start->next == NULL) {
    firstHalf = start;
    secondHalf = NULL;
  }
  else {
    slow = start;
    fast = slow;
    while(fast != NULL) {
      fast = fast->next;
      if (fast != NULL) {
	fast = fast->next;
	slow = slow->next;
      }
    }
    firstHalf = start;
    secondHalf = slow->next;
    slow->next = NULL;
  }
}

void setAvgDist(tile_map_t* map, tile_dist_node_t* theNode) {
  int offset = 0;
  double avgDist = 0;
  int x = theNode->storedTile->tilePosition->posX;
  int y = theNode->storedTile->tilePosition->posY;
  //then find distance of nearest wall directly above
  //suddenly unsure about which dimension is rows or cols
  while(!isAWall(getTileFromMapCord(map, y, x + offset))) {
    offset++;
  }
  avgDist = offset;
    
  offset = 0;
  while(!isAWall(getTileFromMapCord(map, y, x - offset))) {
    offset++;
  }
  avgDist += offset;

  offset = 0;
  while(!isAWall(getTileFromMapCord(map, y + offset, x))) {
    offset++;
  }
  avgDist += offset;

  offset = 0;
  while(!isAWall(getTileFromMapCord(map, y - offset,x))) {
    offset++;
  }
  avgDist += offset;
    
  avgDist /= 4;
  theNode->dVal = avgDist;
}

int isAWall(tile_t* tile) {
  if (tile->isWall == 0) {
    return 0;
  }
  else {
    return 1;
  }
}

void appendToTile_dist_list(tile_dist_list_t* list, tile_dist_node_t* new) {
  if (list->end != NULL) {
    list->end->next = new;
  }
  else if (list->start == NULL) {
    list->start = new;
    new->prev = NULL;
  }
  list->end = new;
  new->next = NULL;
}

void prependToTile_dist_list(tile_dist_list_t* list, tile_dist_node_t* new) {
  if (list->start != NULL) {
    list->start->prev = new;
  }
  else if (list->end == NULL) {
    list->end = new;
    new->next = NULL;
  }
  list->start = new;
  new->prev = NULL;
}

void removeFromTile_dist_list(tile_dist_list_t* list, tile_t * tile) {
  //from begining search
  tile_dist_node_t* node = list->start;
  //just doing address comparison I think, should work, might need to add ids'
  //could also do some fancy math to take a pos and map to a unique number
  //hoping this will work though
  while(node != NULL && node->storedTile != tile) {
    node = node->next;
  }
  if (node != NULL) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  }
  else {
    fprintf(stderr, "Error on attempting to remove a tile from list, tile not in list\n");
  }
}

tile_dist_list_t* createTile_dist_list() {
  tile_dist_list_t* newList = malloc(sizeof(tile_dist_list_t));
  newList->start = NULL;
  newList->end = NULL;
  return newList;
}

tile_dist_node_t* createTile_dist_node(tile_t* tile) {
  tile_dist_node_t* newNode = malloc(sizeof(tile_dist_node_t));
  newNode->dVal = 0;
  newNode->storedTile = tile;
  newNode->next = NULL;
  newNode->prev = NULL;
  return newNode;       
}

void freeTile_dist_node(tile_dist_node_t* tileNode) {
  free(tileNode);
}

void freeTile_dist_list(tile_dist_list_t* tileList) {
  tile_dist_node_t* current = tileList->start;
  while(current != NULL) {
    current = current->next;
    freeTile_dist_node(current->prev);
  }
  free(tileList);
}
