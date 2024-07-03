  #include <bits/stdc++.h>
#define rep(i, n) for (long long i = 0; i < n; ++i)
#define isodd &1 != 0
#define ll long long
#define int long long
#define ld long double
#define mp make_pair
#define llrp(x, y) ((ll)round(exp(x, y)))
#define all(v) v.begin(), v.end()
#define IOS                       \
    ios_base::sync_with_stdio(0); \
    cin.tie(0);                   \
    cout.tie(0)
#define pb push_back

using namespace std;

class Node{
public:
	bool is_leaf;
	ll order;
	Node* next_leaf_ptr;
    
    vector<Node*> ptrs;
    vector<ll> keys;

    Node(ll order, bool is_leaf) {
        this->order=order;
        this->is_leaf=is_leaf;
        this->next_leaf_ptr=nullptr;
    }
};

Node* Root=nullptr;

ll internal_node_order=3;
ll leaf_node_order=2;

int get_ceil(ll tmp){
return (tmp/2)+(tmp%2);
}

Node* get_leaf_node_to_be_inserted(ll insertion_value)
{
	if(Root==nullptr)
	{
		Root=new Node(leaf_node_order,true);
		return Root;
	}
	Node* tmp_node=Root;
	while(!tmp_node->is_leaf)
	{
		ll tmp=lower_bound(all(tmp_node->keys),insertion_value)-(tmp_node->keys).begin();
		tmp_node=tmp_node->ptrs[tmp];
	}

	return tmp_node;
}

Node* get_parent_of_node(Node* tmp_root_node, Node* child)
{ 
	if(tmp_root_node==child || tmp_root_node->is_leaf)
		return nullptr;

	for(auto tmp_root_node_child: tmp_root_node->ptrs)
	{
		if(tmp_root_node_child==child)
			return tmp_root_node;

		Node* possible_parent=get_parent_of_node(tmp_root_node_child,child);
		if(possible_parent!=nullptr)
			return possible_parent;
	}
	// Never executed
	return nullptr;
}

void insert_in_parent_node(Node* childNode,ll newKey,Node* newNodePtr)
{
	Node* parent=get_parent_of_node(Root, childNode);
	
	if(parent==nullptr)
	{
		Node* tmp_root=new Node(internal_node_order,false);
		tmp_root->keys.pb(newKey);
		tmp_root->ptrs.pb(childNode);
		tmp_root->ptrs.pb(newNodePtr);
		Root=tmp_root;
		return;
	}

	ll ind=upper_bound(all(parent->keys),newKey)-parent->keys.begin();
    parent->keys.insert(parent->keys.begin()+ind,newKey);
    parent->ptrs.insert(parent->ptrs.begin()+ind+1,newNodePtr);

    if(parent->keys.size()>=parent->order)
	{
		Node* new_node=new Node(internal_node_order,false);
		ll ind_to_start_split=((parent->order)+1)/2;

		for(ll j=ind_to_start_split;j<parent->keys.size();j++)
		{
			new_node->keys.pb(parent->keys[j]);
			new_node->ptrs.pb(parent->ptrs[j]);
		}
		new_node->ptrs.pb(parent->ptrs.back());
		
		parent->keys.resize(ind_to_start_split);
		parent->ptrs.resize(ind_to_start_split);

		ll tmp=parent->keys.back();
		parent->keys.pop_back();

		insert_in_parent_node(parent,tmp,new_node);
	}
    return;
}

void insert(ll val)
{
	Node* node_to_inserted=get_leaf_node_to_be_inserted(val);

	ll ind=upper_bound(all(node_to_inserted->keys),val)-node_to_inserted->keys.begin();
    node_to_inserted->keys.insert(node_to_inserted->keys.begin()+ind,val);
    node_to_inserted->ptrs.insert(node_to_inserted->ptrs.begin()+ind,nullptr);

	if(node_to_inserted->keys.size()>node_to_inserted->order)
	{
		Node* new_node=new Node(leaf_node_order,true);
		new_node->next_leaf_ptr=node_to_inserted->next_leaf_ptr;
        node_to_inserted->next_leaf_ptr=new_node;

		ll ind_to_start_split=ceil((1.00*(node_to_inserted->order+1))/2);

		for(ll j=ind_to_start_split;j<node_to_inserted->keys.size();j++)
		{
			new_node->keys.pb(node_to_inserted->keys[j]);
			new_node->ptrs.pb(node_to_inserted->ptrs[j]);
		}
		node_to_inserted->keys.resize(ind_to_start_split);
		node_to_inserted->ptrs.resize(ind_to_start_split);
		insert_in_parent_node(node_to_inserted,node_to_inserted->keys.back(),new_node);
	}
	return;
}

void delete_from_internal_node(Node* toBeDeletedFromNode, int val)
{
    ll val_ind=lower_bound(all(toBeDeletedFromNode->keys),val)-toBeDeletedFromNode->keys.begin();

    toBeDeletedFromNode->keys.erase(toBeDeletedFromNode->keys.begin()+val_ind);
	toBeDeletedFromNode->ptrs.erase(toBeDeletedFromNode->ptrs.begin()+val_ind);

	if(toBeDeletedFromNode->ptrs.size()>=get_ceil(toBeDeletedFromNode->order))
		return;

	Node* parent_node=get_parent_of_node(Root, toBeDeletedFromNode);
    
    if(parent_node==nullptr){
    	if(toBeDeletedFromNode->keys.size()==0)
    		Root=toBeDeletedFromNode->ptrs[0];
    	return;
    }
    
    int leftSibling,rightSibling;
    for(ll i=0;i<parent_node->ptrs.size();i++)
    {
    	if(parent_node->ptrs[i]==toBeDeletedFromNode)
    	{
    		leftSibling=i-1;
            rightSibling=i+1;
            break;
        }
    }

    if(leftSibling>=0 && parent_node->ptrs[leftSibling]->keys.size()>=get_ceil(parent_node->order))
    {
        Node* left_sibling=parent_node->ptrs[leftSibling];

    	toBeDeletedFromNode->keys.insert(toBeDeletedFromNode->keys.begin(),parent_node->keys[leftSibling]);
        toBeDeletedFromNode->ptrs.insert(toBeDeletedFromNode->ptrs.begin(),left_sibling->ptrs.back());

        parent_node->keys[leftSibling]=left_sibling->keys.back();

        left_sibling->keys.pop_back();
        left_sibling->ptrs.pop_back();

        return;
    }

    if(rightSibling<parent_node->ptrs.size() && parent_node->ptrs[rightSibling]->keys.size()>=get_ceil(parent_node->order))
    {
        Node* right_sibling=parent_node->ptrs[rightSibling];

    	toBeDeletedFromNode->keys.pb(parent_node->keys[rightSibling-1]);
        toBeDeletedFromNode->ptrs.pb(right_sibling->ptrs[0]);

        parent_node->keys[rightSibling-1]=right_sibling->keys[0];

        right_sibling->keys.erase(right_sibling->keys.begin());
        right_sibling->ptrs.erase(right_sibling->ptrs.begin());
       
        return;
    }
    

    if(leftSibling>=0)
    {
    	Node* left_sibling=parent_node->ptrs[leftSibling];
        
        toBeDeletedFromNode->keys.insert(toBeDeletedFromNode->keys.begin(),parent_node->keys[leftSibling]);
        
        toBeDeletedFromNode->ptrs.insert(toBeDeletedFromNode->ptrs.begin(),left_sibling->ptrs.back());
        left_sibling->ptrs.pop_back();

    	while(left_sibling->keys.size())
    	{
    		toBeDeletedFromNode->keys.insert(toBeDeletedFromNode->keys.begin(),left_sibling->keys.back());
    		toBeDeletedFromNode->ptrs.insert(toBeDeletedFromNode->ptrs.begin(),left_sibling->ptrs.back());
    		left_sibling->keys.pop_back();
    		left_sibling->ptrs.pop_back();
    	}
        delete_from_internal_node(parent_node,parent_node->keys[leftSibling]);

    }
    else
    {
    	Node* right_sibling=parent_node->ptrs[rightSibling];

        right_sibling->keys.insert(right_sibling->keys.begin(),parent_node->keys[rightSibling-1]);

        right_sibling->ptrs.insert(right_sibling->ptrs.begin(),toBeDeletedFromNode->ptrs.back());
        toBeDeletedFromNode->ptrs.pop_back();

    	while(toBeDeletedFromNode->keys.size())
    	{
    		right_sibling->keys.insert(right_sibling->keys.begin(),toBeDeletedFromNode->keys.back());
    		right_sibling->ptrs.insert(right_sibling->ptrs.begin(),toBeDeletedFromNode->ptrs.back());
    		toBeDeletedFromNode->keys.pop_back();
    		toBeDeletedFromNode->ptrs.pop_back();
    	}
        delete_from_internal_node(parent_node,parent_node->keys[rightSibling-1]);
    }
}

void delete_value(ll val)
{
	int leftSibling, rightSibling;

	Node* tmp_node=Root;

    while (!(tmp_node->is_leaf)) {
		for (int i = 0; i < tmp_node->keys.size(); i++) {
			leftSibling = i - 1;
			rightSibling = i + 1;

			if (val <= tmp_node->keys[i]) {
				tmp_node = tmp_node->ptrs[i];
				break;
			}
			if (i == tmp_node->keys.size() - 1) {
				leftSibling = i;
				rightSibling = i + 2;
				tmp_node = tmp_node->ptrs[i+1];
				break;
			}
		}
	}

	tmp_node->keys.erase(lower_bound(all(tmp_node->keys),val));
	tmp_node->ptrs.pop_back();

	if(tmp_node->keys.size()>=get_ceil(tmp_node->order))
		return;
    
    Node* parent_node=get_parent_of_node(Root, tmp_node);
    if(parent_node==nullptr)return;

    if(leftSibling>=0 && parent_node->ptrs[leftSibling]->keys.size()>=get_ceil(parent_node->ptrs[leftSibling]->order)+1)
    {
        Node* left_sibling=parent_node->ptrs[leftSibling];

    	tmp_node->keys.insert(tmp_node->keys.begin(),left_sibling->keys.back());
        tmp_node->ptrs.insert(tmp_node->ptrs.begin(),nullptr);

        left_sibling->keys.pop_back();
        left_sibling->ptrs.pop_back();
        parent_node->keys[leftSibling]=left_sibling->keys.back();

        return;
    }

    if(rightSibling<parent_node->ptrs.size() && parent_node->ptrs[rightSibling]->keys.size()>=get_ceil(parent_node->ptrs[rightSibling]->order)+1)
    {
        Node* right_sibling=parent_node->ptrs[rightSibling];

    	tmp_node->keys.pb(right_sibling->keys[0]);
        tmp_node->ptrs.pb(nullptr);

        right_sibling->keys.erase(right_sibling->keys.begin());
        right_sibling->ptrs.pop_back();
        parent_node->keys[rightSibling-1]=tmp_node->keys.back();
       
        return;
    }

    if(leftSibling>=0)
    {
    	Node* left_sibling=parent_node->ptrs[leftSibling];

    	while(left_sibling->keys.size())
    	{
    		tmp_node->keys.insert(tmp_node->keys.begin(),left_sibling->keys.back());
    		tmp_node->ptrs.insert(tmp_node->ptrs.begin(),left_sibling->ptrs.back());
    		left_sibling->keys.pop_back();
    		left_sibling->ptrs.pop_back();
    	}
        delete_from_internal_node(parent_node,parent_node->keys[leftSibling]);

    }
    else
    {
    	Node* right_sibling=parent_node->ptrs[rightSibling];

    	while(tmp_node->keys.size())
    	{
    		right_sibling->keys.insert(right_sibling->keys.begin(),tmp_node->keys.back());
    		right_sibling->ptrs.insert(right_sibling->ptrs.begin(),tmp_node->ptrs.back());
    		tmp_node->keys.pop_back();
    		tmp_node->ptrs.pop_back();
    	}
        delete_from_internal_node(parent_node,parent_node->keys[rightSibling-1]);
    }
    return;
}

void print_tree()
{
	queue<pair<Node*,ll>> q;
	q.push(make_pair(Root,0));

	while(!q.empty())
	{
		ll height=q.front().second;
		ll sz=q.size();
        
        cout<<"Level"<<height<<"\n";
        
        while(sz--)
        {
		    
            Node* tmp=q.front().first;
            q.pop();

		    if(!tmp->is_leaf)
		    for(auto x: tmp->ptrs)
			    q.push({x,height+1});

		    for(auto x: tmp->keys)
			     cout<<x<<" ";

            cout<<"\n";
        }

        cout<<"\n";
	}

	cout<<"\n\n\n";
}



signed main()
{

    // insert(20);
    // delete_value(20);

    for(ll i=10;i<=20;i+=2)
    	{insert(i);print_tree();}

    for(ll i=20;i>=10;i-=2)
    	{delete_value(i);print_tree();}


    // insert(11);
    // insert(14);
    // insert(25);
    // insert(30);
    // insert(12);
    // insert(22);
    // insert(23);
    // insert(24);
    // print_tree();

   
    // delete_value(25);
    // // print_tree();     

    // delete_value(24);
    // // print_tree();

    // delete_value(23);
    // // print_tree();

    // delete_value(22);
    // // print_tree();

    // delete_value(30);
    print_tree();

    // delete_value(25);
    // print_tree();




    return 0;
}
