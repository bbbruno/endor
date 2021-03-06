//===============================================================================//
// Name			: aograph.cpp
// Author(s)	: Barbara Bruno, Yeshasvi Tirupachuri V.S.
// Affiliation	: University of Genova, Italy - dept. DIBRIS
// Description	: AND-OR graph
//===============================================================================//

#include "aograph.h"

//! constructor of class Path
//! @param[in] cost 	initial cost of the path
//! @param[in] index 	unique index of the path
Path::Path(int cost, int index)
{
    pIndex = index;
    pCost = cost;
    pComplete = false;
}

//! copy constructor of class Path
//! @param[in] &toBeCopied  path to be copied
//! @param[in] index        unique index of the path
Path::Path(const Path &toBeCopied, int index)
{
    pIndex = index;
    pCost = toBeCopied.pCost;
    pComplete = false;
    pathNodes = toBeCopied.pathNodes;
    pathArcs = toBeCopied.pathArcs;
    checkedNodes = toBeCopied.checkedNodes;
}

//! display path information
void Path::printPathInfo()
{
    cout<<"Info of path: " <<pIndex <<endl;
    //DEBUG: cout<<"Is complete? " <<boolalpha <<pComplete <<endl;
    cout<<"Total cost: " <<pCost <<endl;
    cout<<"Hyperarcs in path: ";
    for (int i=0; i< (int)pathArcs.size(); i++)
        cout<<pathArcs[i] <<" ";
    cout<<endl <<"Nodes in path:" <<endl;
    for (int i=0; i< (int)pathNodes.size(); i++)
    {
        cout<<pathNodes[i]->nName <<" ";
        //DEBUG: cout<<"checked? " <<boolalpha <<checkedNodes[i];
        if (checkedNodes[i] == true)
            cout<<"- done";
        cout<<endl;
    }
    cout<<endl;
}

//! add a node in the path
//! @param[in] node     node to add to the path
void Path::addNode(AOnode* node)
{
    // update the nodes in the path
    pathNodes.push_back(node);
    checkedNodes.push_back(false);
    
    // N.B. the cost of the path is updated when the node is checked
}

//! update the path information (when a node is solved)
//! @param[in] nameNode     name of the node
//! @param[in] cost         cost to subtract from the path cost
void Path::updatePath(string nameNode, int cost)
{
   // check whether the node is in the path
    for(int i=0; i < (int)pathNodes.size(); i++)
    {
        // keep track of solved nodes
        if (pathNodes[i]->nName == nameNode)
            checkedNodes[i] = true;
    }
    // update the cost of the path        
    pCost = pCost - cost;
    
    cout<<"Path: " <<pIndex <<endl;
    cout<<"Updated path cost: " <<pCost <<endl;
}

//! find the feasible node to suggest
//! @return node to suggest
AOnode* Path::suggestNode()
{
    AOnode* selection = NULL;
    
    // iterate on the nodes in the path, from the last to the first one
    for (int i = (int)pathNodes.size()-1; i > -1; i--)
    {
        // rationale for the suggestion:
        // 1. move along the path from the leaves to the head
        // 2. choose the first feasible & not-solved node
        if (checkedNodes[i] == false)
        {
            if (pathNodes[i]->nFeasible == true)
            {
                selection = pathNodes[i];
                break;
            }
        }
    }
    
    // raise an error if the suggested node is NULL
    if (selection == NULL)
        cout<<"[ERROR] No suggestion possible." <<endl;
    
    return selection;
}

//! add a node in the graph
//! @param[in] nameNode    name of the node
//! @param[in] cost        generic node cost
void AOgraph::addNode(string nameNode, int cost)
{
    // create the node
    AOnode toAdd(nameNode, cost);
    
    // add it to the set of nodes in the graph
    graph.push_back(toAdd);
}

//! find a node by name
//! @param[in] nameNode    name of the node
//! @return	               pointer to the node with given name
AOnode* AOgraph::findByName(string nameNode)
{
    AOnode* temp = NULL;
    
    for (int i=0; i< (int)graph.size(); i++)
    {
        if (graph[i].nName == nameNode)
        {
            temp = &graph[i];
            break;
        }
    }
    
    // issue a warning if the node has not been found
    if (temp == NULL)
        cout<<"[Warning] Name not found."
            <<"Did you really look for " <<nameNode <<"?" <<endl;
    return temp;
}

//! update the feasibility status of the nodes in the graph
void AOgraph::updateNodeFeasibility()
{
    for (int i=0; i< (int)graph.size(); i++)
        graph[i].isFeasible();        
}
		
//! compute the cost to add to a path
//! @param[in] node     reference to the node to use for cost computation
//! @param[in] hIndex   index of the node's hyperarc to use for cost computation
//! @return             cost to add to a path
int AOgraph::computeAddCost(AOnode &node, int hIndex)
{
    // 1. the cost is to be ADDED to the cost of the path
    // 2. cost = node.nCost + node.arcs[hIndex].hCost
    int cost = 0;
    
    // raise an error if the hyperarc index is out of bounds
    if (hIndex >= (int)node.arcs.size())
    {
        cout<<"[ERROR] The node has only " <<node.arcs.size() <<" hyperarcs."
            <<"Hyperarc index " <<hIndex <<" does not exist." <<endl;
        return -1;
    }
        
    // if hIndex == -1, the node is terminal
    if (hIndex == -1)
        cost = node.nCost;
    // otherwise, the cost to add is given by node.cost and hyperarc.cost
    else
        cost = node.nCost + node.arcs[hIndex].hCost;
    //DEBUG:cout<<"Node: " <<node.nName <<" - Cost: " <<cost <<endl;
    
    return cost;
}

//! generate all possible paths navigating the graph
void AOgraph::generatePaths()
{
    // if the head node is NULL, there are no paths to generate
    if (head == NULL)
    {
        cout<<"[WARNING] There is no graph to navigate (head == NULL)." <<endl;
        return;
    }
    
    // otherwise, create a path with the head node
    Path* newPath = new Path(0,0);
    newPath->addNode(head);
    paths.push_back(*newPath);
        
    // iterate through the paths until they're all complete
    AOnode* currentNode = head;
    while(1)
    {
        // find the first not-complete path
        bool allComplete = true;
        int currentPathIndex;
        for (int i=0; i< (int)paths.size(); i++)
        {
            if (paths[i].pComplete == false)
            {
                allComplete = false;
                currentPathIndex = i;
                break;
            }
        }
        // if all paths are complete, the generation is done
        if (allComplete == true)
            return;
            
        // find the first not-checked node in the open path
        bool allChecked = true;
        int currentNodeIndex;
        for (int i=0; i< (int)paths[currentPathIndex].checkedNodes.size(); i++)
        {
            if (paths[currentPathIndex].checkedNodes[i] == false)
            {
                allChecked = false;
                currentNode = paths[currentPathIndex].pathNodes[i];
                currentNodeIndex = i;
                break;
            }
        }
        // if all nodes are checked, the path is complete
        if (allChecked == true)
            paths[currentPathIndex].pComplete = true;
        else
        {
            // if the current node is terminal:
            // 1. check it
            // 2. update the path cost with the current node cost
            if (currentNode->arcs.size() == 0)
            {
                paths[currentPathIndex].checkedNodes[currentNodeIndex] = true;
                int cost = computeAddCost(*currentNode, -1);
                paths[currentPathIndex].pCost = paths[currentPathIndex].pCost + cost;
            }
            
            // if the current node has only one hyperarc:
            // 1. check it
            // 2. add the hyperarc index to the path
            // 3. update the path cost with the current node+only_hyperarc cost
            // 4. add its child nodes to the path
            if (currentNode->arcs.size() == 1)
            {
                paths[currentPathIndex].checkedNodes[currentNodeIndex] = true;
                paths[currentPathIndex].pathArcs.push_back(currentNode->arcs[0].hIndex);
                int cost = computeAddCost(*currentNode, 0);
                paths[currentPathIndex].pCost = paths[currentPathIndex].pCost + cost;
                for (int i=0; i< (int)currentNode->arcs[0].children.size(); i++)
                    paths[currentPathIndex].addNode(currentNode->arcs[0].children[i]);
            }
            
            // if the current node has more than one hyperarc:
            // 1. create (numArcs-1) copies of the current path
            // 2. check the current node in the copies
            // 3. add the other_hyperarc index to the copies
            // 4. update the path cost with the current node+other_hyperarc cost
            // 5. add the child nodes of the last (numArcs-1) arcs to the copies
            // 6. check the current node in the current path
            // 7. add the first_hyperarc index to the current path
            // 8. update the path cost with the current node+first_hyperarc cost
            // 9. add the child nodes of the first arcs to the current path
            if (currentNode->arcs.size() > 1)
            {
                int numCopies = currentNode->arcs.size()-1;
                for (int i=0; i<numCopies; i++)
                {
                    newPath = new Path(paths[currentPathIndex], paths.size());
                    newPath->checkedNodes[currentNodeIndex] = true;
                    newPath->pathArcs.push_back(currentNode->arcs[i+1].hIndex);
                    int cost = computeAddCost(*currentNode, i+1);
                    newPath->pCost = newPath->pCost + cost;
                    for (int j=0; j< (int)currentNode->arcs[i+1].children.size(); j++)
                        newPath->addNode(currentNode->arcs[i+1].children[j]);
                    paths.push_back(*newPath);
                }
                paths[currentPathIndex].checkedNodes[currentNodeIndex] = true;
                paths[currentPathIndex].pathArcs.push_back(currentNode->arcs[0].hIndex);
                int cost = computeAddCost(*currentNode, 0);
                paths[currentPathIndex].pCost = paths[currentPathIndex].pCost + cost;
                for (int i=0; i< (int)currentNode->arcs[0].children.size(); i++)
                    paths[currentPathIndex].addNode(currentNode->arcs[0].children[i]);
            }
        }
    }
}

//! set up a graph
void AOgraph::setupGraph()
{
    // update the feasibility status of the nodes in the graph
    updateNodeFeasibility();
    //DEBUG:printGraphInfo();
    
    // generate all paths navigating the graph
    generatePaths();
    // set the "checked" property of the nodes in the paths to false
    // NOTE: during execution, "checked" is used to mark the solved nodes
    for (int i=0; i < (int)paths.size(); i++)
        for (int j=0; j < (int)paths[i].checkedNodes.size(); j++)
            paths[i].checkedNodes[j] = false;

    for (int i=0; i < (int)paths.size(); i++)
        paths[i].printPathInfo();
            
    // identify the first suggestion to make (long-sighted strategy chosen BY DEFAULT)
    suggestNext(true);
}

//! find the hyperarc connecting a parent to a child node
//! @param[in] parent   reference to the parent node
//! @param[in] child    reference to the child node
//! @return             index of the hyperarc connecting the parent to the child
HyperArc* AOgraph::findHyperarc(AOnode &parent, AOnode &child)
{
    HyperArc* temp = NULL;
    
    for (int j=0; j< (int)parent.arcs.size(); j++)
    {
        for (int k=0; k< (int)parent.arcs[j].children.size(); k++)
        {
            if(parent.arcs[j].children[k]->nName == child.nName)
            {
                temp = &parent.arcs[j];
                //DEBUG:cout<<"Found index: " <<temp->hIndex <<endl;
                break;
            }
        }
    }
    
    /* DEBUG
    // raise a warning if no hyperarc was found
    if (temp == NULL)
        cout<<"[WARNING] There is no hyperarc connecting " <<parent.nName
            <<" to " <<child.nName <<"." <<endl;
    */
    
    return temp;    
}

//! compute the overall update cost (intermediate step to update the path cost)
//! @param[in] node     reference to the node to use for cost computation
//! @return             overall update cost (to subtract from the path cost)
int AOgraph::computeOverallUpdate(AOnode &node)
{    
    // 1. the cost is to be SUBTRACTED from the cost of the path
    // 2. the cost is computed as:
        // a. pathsCosts = set of the costs of the hyperarcs TO the current node
        // b. cost = max(pathsCosts)
        
    // N.B. the cost is to be subtracted from path[i] as:
    // 1. toSubtract = node.nCost + abs(pathsCosts[path_i] - cost);
    
    vector<int> pathsCosts;
    
    // for each parent node, find the cost of the hyperarc to the current node
    for (int i=0; i< (int)node.parents.size(); i++)
    {
        HyperArc* arc = findHyperarc(*node.parents[i], node);
        if (arc != NULL)
        {
            int arcCost = arc->hCost;
            pathsCosts.push_back(arcCost);
        }
    }
    /* DEBUG
    cout<<"maxUpdate is the max of: ";
    for (int i=0; i< (int)pathsCosts.size(); i++)
        cout<<pathsCosts[i] <<" ";
    cout<<endl;
    */
    
    // find the maximum in pathsCosts
    int cost = pathsCosts[0];
    for (int i=1; i< (int)pathsCosts.size(); i++)
        if (pathsCosts[i] > cost)
            cost = pathsCosts[i];
    //DEBUG:cout<<"maxUpdate = " <<cost <<endl;
    
    return cost;
}

//! update all paths (update path costs when a node is solved)
//! @param[in] solved       reference to the solved node (to use for paths costs update)
void AOgraph::updatePaths(AOnode &solved)
{
    // update the path information (cost) of EACH path as:
    // toSubtract = solved.nCost + overall_update - path_i_update;
    // path[i].cost = path[i].cost - toSubtract;
    
    pIndices.clear();
    pUpdate.clear();
    int toSubtract = solved.nCost + computeOverallUpdate(solved);
    //DEBUG:cout<<"solved.nCost = " <<solved.nCost <<endl;
    //DEBUG:cout<<"maxUpdate = " <<computeOverallUpdate(solved) <<endl;
    
    // find all paths which include the solved node
    vector<int> withChild;
    for (int i=0; i < (int)paths.size(); i++)
        for (int j=0; j< (int)paths[i].pathNodes.size(); j++)
            if(paths[i].pathNodes[j]->nName == solved.nName)
                withChild.push_back(i);
    /* DEBUG
    cout<<"Paths with solved node: ";
    for (int i=0; i < (int)withChild.size(); i++)
        cout<<"Path index: " <<withChild[i] <<" ";
    cout<<endl;
    */
    
    // find the paths including the solved node AND any of its parents
    vector<int> withBoth;
    int toAdd;
    for (int i=0; i < (int)withChild.size(); i++)
    {
        for (int j=0; j < (int)solved.parents.size(); j++)
            for (int k=0; k < (int)paths[withChild[i]].pathNodes.size(); k++)
                if(paths[withChild[i]].pathNodes[k]->nName == solved.parents[j]->nName)
                    toAdd = withChild[i];
        // make sure that paths are added only ONCE
        if ( std::find(withBoth.begin(), withBoth.end(), toAdd) == withBoth.end() )
            withBoth.push_back(toAdd);
    }
    /* DEBUG
    cout<<"Paths with solved node & any parent: ";
    for (int i=0; i < (int)withBoth.size(); i++)
        cout<<"Path index: " <<withBoth[i] <<" ";
    cout<<endl;
    */

    // find the paths containing a DIRECT LINK between the solved node and a parent
    for (int i=0; i < (int)withBoth.size(); i++)
    {
        for (int j=0; j < (int)paths[withBoth[i]].pathNodes.size(); j++)
        {
            HyperArc* arc = findHyperarc(*paths[withBoth[i]].pathNodes[j], solved);
                    
            // update the path cost (if there is a direct link in THIS path)
            if (arc != NULL)
            {
                for (int k=0; k < (int)paths[withBoth[i]].pathArcs.size(); k++)
                {
                    if (paths[withBoth[i]].pathArcs[k] == arc->hIndex)
                    {
                        // compute "path_i_update"
                        int pathUpdate = arc->hCost;
                        //DEBUG:cout<<"pathUpdate = " <<pathUpdate <<endl;
                        int thisSubtract = toSubtract - pathUpdate;
                        
                        // update the cost of the path
                        paths[withBoth[i]].updatePath(solved.nName, thisSubtract);
                        
                        // save the index & subtracted cost of the updated path
                        pIndices.push_back(withBoth[i]);
                        pUpdate.push_back(pathUpdate);
                        
                        break;
                    }
                }
            }
        }
    }
}

//! find the optimal path (long-sighted strategy)
//! @return index of the optimal path (minimum cost)
int AOgraph::findOptimalPath()
{
    // raise an error if there are no paths
    if (paths.size() == 0)
    {
        cout<<"[ERROR] There are no paths navigating the graph. "
            <<"Did you run generatePaths()?" <<endl;
        return -1;
    }
    
    int index = 0;
    int cost = paths[0].pCost;
    for (int i=0; i< (int)paths.size(); i++)
    {
        // raise an error if there are not-complete paths
        if (paths[i].pComplete == false)
        {
            cout<<"[ERROR] The paths navigating the graph are not complete. "
                <<"Did you run generatePaths()?" <<endl;
            return -1;
        }
        
        if (paths[i].pCost < cost)
        {
            cost = paths[i].pCost;
            index = i;
        }
    }
    cout<<"The optimal path is: " <<index <<endl;
    paths[index].printPathInfo();
    
    return index;
}

//! constructor of class AOgraph
//! @param[in] name 	name of the graph
AOgraph::AOgraph(string name)
{
    gName = name;
    head = NULL;
    
    //DEBUG:printGraphInfo();
}

//! load the graph description from a file
//! @param[in] fileName    name of the file with the graph description
void AOgraph::loadFromFile(string fileName)
{
    // raise an error if the graph is not empty
    if (graph.size() != 0)
    {
        cout<<"[ERROR] The graph is not empty."
            <<"Do you really want to overwrite the current graph?" <<endl;
        return;
    }            
    
    ifstream graphFile(fileName.c_str());
	cout <<"Loading graph description from file: " <<fileName <<endl;
    
	while (!graphFile.eof())
	{
        // the first line contains:
        // 1. the name of the graph
        // 2. the number N=numNodes of nodes
        // 3. the name of the head node (corresponding to the final assembly)
        string name;
        int numNodes;
        string headName;
        
        graphFile >>name >>numNodes >>headName;
        if (!graphFile)
			break;
        gName = name;
        
        // the next N lines contain the name and cost of all the nodes in the graph
        string nameNode;
        int cost;
        for (int i=0; i < numNodes; i++)
        {            
            graphFile >>nameNode >> cost;
            if (!graphFile)
                break;
            addNode(nameNode, cost);
        }
        
        // the next ?? lines contain the descriptions of the hyperarcs in the graph
        int hyperarcIndex = 0;
        while (!graphFile.eof())
        {
            AOnode* father;
            string nameFather;
            int numChildren;
            int hyperarcCost;
            vector<AOnode*> childNodes;
            
            graphFile >>numChildren >>nameFather >>hyperarcCost;
            if (!graphFile)
                break;
            father = findByName(nameFather);
            //DEBUG:cout<<"nameFather = " <<nameFather <<endl;
            
            // the next numChildren lines contain the names of the child nodes
            for (int i=0; i < numChildren; i++)
            {
                AOnode* temp;
                string nameChild;
                graphFile >>nameChild;
                if (!graphFile)
                    break;
                temp = findByName(nameChild);
                childNodes.push_back(temp);
            }
            father->addArc(hyperarcIndex, childNodes, hyperarcCost);
            hyperarcIndex = hyperarcIndex+1;
        }
        // identify the head node in the graph
        head = findByName(headName);
    }
    graphFile.close();
    
    // set up the graph (nodes feasibility, paths costs)
    setupGraph();
}

//! display graph information
void AOgraph::printGraphInfo()
{
    cout<<endl;
    cout<<"Info of graph: " <<gName <<endl;
    cout<<"Number of nodes: " <<graph.size() <<endl;
    cout<<"Head node: " <<head->nName <<endl <<endl;
    for (int i=0; i< (int)graph.size(); i++)
        graph[i].printNodeInfo();
    cout<<endl;
}

//! suggest the node to solve
//! @param[in] strategy     "0" = short-sighted, "1" = long-sighted
//! @return                 name of the suggested node
string AOgraph::suggestNext(bool strategy)
{
    // issue a warning if the graph has been solved already
    if (head->nSolved == true)
    {
        cout<<"[WARNING] The graph is solved. No suggestion possible." <<endl;
        return "end";
    }
    
    int optimalPathIndex = 0;
        
    // short-sighted strategy:
    // pick the path which received the highest benefit from the last action
    if (strategy == false)
    {
        // find the path with highest benefit from last action
        for (int i=1; i< (int)pUpdate.size(); i++)
            if (pUpdate[i] > pUpdate[optimalPathIndex])
                optimalPathIndex = pIndices[i];
    }
    // long-sighted strategy:
    // pick the path which minimizes the cost to completion
    if (strategy == true)
        optimalPathIndex = findOptimalPath();

    AOnode* suggestion = paths[optimalPathIndex].suggestNode();
    cout<<"ENDOR suggestion: " <<endl
        <<"Suggested path = " <<optimalPathIndex <<endl
        <<"Suggested node = " <<suggestion->nName <<endl;
    
    return suggestion->nName;
}

//! solve a node, finding it by name
//! @param[in] nameNode    name of the node
void AOgraph::solveByName(string nameNode)
{
    AOnode* solved = findByName(nameNode);
    bool result = solved->setSolved();
    updateNodeFeasibility();
    printGraphInfo();
    
    // report that the graph has been solved if the solved node is the head node
    if (head->nSolved == true)
    {
        cout<<"[REPORT] The graph is solved (head node solved)." <<endl;
        return;
    }
    
    // update the path information (cost) of all paths
    if (result == true)
        updatePaths(*solved);
    cout<<endl <<"Updated paths: " <<endl;
    for(int i=0; i< (int)pUpdate.size(); i++)
        cout<<"Path index: " <<pIndices[i] <<" - Benefit: " <<pUpdate[i] <<endl;
}
