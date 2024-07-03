  #include <bits/stdc++.h>
#define rep(i, n) for (long long i = 0; i < n; ++i)
#define isodd &1 != 0
#define ll long long
#define int long long
#define ld long double
#define mp make_pair
#define llrp(x, y) ((ll)round(exp(x, y)))
#define all(v) v.begin(), v.end()
#define back(v) v.rbegin(), v.rend()
#define IOS                       \
    ios_base::sync_with_stdio(0); \
    cin.tie(0);                   \
    cout.tie(0)
#define pb push_back

using namespace std;


class Bucket {
    public:
        int depth;
        vector<int> elements;
        Bucket(int depth): depth(depth){
            this->elements.empty();
        };
};

int bucket_size;
int global_depth=0;
vector<Bucket*> directory;
bool depth_decreased=false;



int get_ind_of_bucket_to_be_changed(int element);


// functions realted to insertion
void insert_element(int element);
void increase_depth_of_bucket(int ind_in_directory);
void increase_global_depth(int ind_in_directory);

// functions realted to deletion
void delete_element(int element);
void decrease_depth_of_bucket(int ind_in_directory);
void decrease_global_depth(int ind_in_directory);


int get_ind_of_bucket_to_be_changed(int element)
{
    int ind_of_bucket_to_be_inserted=0;
    for(ll i=0;i<global_depth;i++)
        if(element&(1<<i))
        ind_of_bucket_to_be_inserted+=(1<<i);

    return ind_of_bucket_to_be_inserted;
}

void increase_depth_of_bucket(int ind_in_directory)
{

    Bucket* bucket_to_be_changed=(directory[ind_in_directory]);
    int curr_local_depth=bucket_to_be_changed->depth;
    int new_local_depth=curr_local_depth+1;

    Bucket* tmp1_bucket=new Bucket(new_local_depth);
    Bucket* tmp2_bucket=new Bucket(new_local_depth);

    for(ll i=0;i<(1<<global_depth);i++)
    {
        if(directory[i]==bucket_to_be_changed)
        {
            if(i&(1<<(curr_local_depth)))
                directory[i]=tmp2_bucket;
            else
                directory[i]=tmp1_bucket;
        }
    }

    for(auto x: bucket_to_be_changed->elements)
        insert_element(x);
    delete bucket_to_be_changed;
}

void increase_global_depth(int ind_in_directory)
{
    vector<Bucket*> tmp_directory;

    for(ll i=0;i<2;i++)
    {
        for(ll j=0;j<(1<<global_depth);j++)
            tmp_directory.pb(directory[j]);
    }

    Bucket* data_to_be_copied_from=directory[ind_in_directory];

    tmp_directory[ind_in_directory]=new Bucket(global_depth+1);
    tmp_directory[ind_in_directory+(1<<global_depth)]=new Bucket(global_depth+1);
    global_depth++;

    directory=tmp_directory;

    for(auto x: data_to_be_copied_from->elements)
        {insert_element(x);}

    delete data_to_be_copied_from;
}

void insert_element(int element)
{
    int ind_of_bucket_to_be_insert=get_ind_of_bucket_to_be_changed(element);


    Bucket* bucket_to_be_inserted_on=directory[ind_of_bucket_to_be_insert];

    if(bucket_to_be_inserted_on->elements.size()<bucket_size)
    {bucket_to_be_inserted_on->elements.pb(element);return;}

    if(bucket_to_be_inserted_on->depth==global_depth)
    {increase_global_depth(ind_of_bucket_to_be_insert);}
    else
    {increase_depth_of_bucket(ind_of_bucket_to_be_insert);}

    insert_element(element);
}

void decrease_global_depth()
{
    ll max_local_depth=0;

    for(auto x: directory)max_local_depth=max(max_local_depth,x->depth);

    if(global_depth>max_local_depth)
    {
        global_depth--;
        directory.resize((1<<global_depth));
    }
}

void decrease_depth_of_bucket(int ind_in_directory)
{

    Bucket* bucket_to_be_changed=(directory[ind_in_directory]);
    Bucket* matching_bucket=(directory[ind_in_directory^(1<<((bucket_to_be_changed->depth)-1))]);

    if(bucket_to_be_changed->elements.size()+matching_bucket->elements.size()<=bucket_size)
    {
        depth_decreased=true;
        for(ll i=0;i<(1<<global_depth);i++)
        {
            if(directory[i]==bucket_to_be_changed)
            directory[i]=matching_bucket;
        }

        matching_bucket->depth=(matching_bucket->depth)-1;
        for(auto x: bucket_to_be_changed->elements)
            insert_element(x);
    }

    decrease_global_depth();
}

void delete_element(int element)
{
    int ind_of_bucket_to_be_delete=get_ind_of_bucket_to_be_changed(element);

    Bucket* bucket_to_be_deleted_on=directory[ind_of_bucket_to_be_delete];
    
    bucket_to_be_deleted_on->elements.erase(find(all(bucket_to_be_deleted_on->elements),element));

    if(bucket_to_be_deleted_on->depth>0)
    {
        decrease_depth_of_bucket(ind_of_bucket_to_be_delete);
    }
}

void print_directory(){
    
    cout<<"Global depth of bucket ";
    cout<<global_depth<<"\n";

    cout<<"\n";

    for(ll i=0;i<(1<<global_depth);i++)
    {
        cout<<"Bucket at index "<<i<<"\n";
        
        cout<<"Local depth ";
        cout<<directory[i]->depth<<"\n";

        cout<<"Elements ";
        for(auto x: directory[i]->elements)
            cout<<x<<" ";
        cout<<"\n\n";
    }
}

signed main()
{
    cin>>bucket_size;
    
    for(ll i=0;i<(1<<global_depth);i++)
        {directory.pb(new Bucket(0));}


    for(ll i=10;i<=20;i+=2)
    {insert_element(i);print_directory();}

    for(ll i=20;i>=10;i-=2)
    {delete_element(i);print_directory();}

    return 0;
}
