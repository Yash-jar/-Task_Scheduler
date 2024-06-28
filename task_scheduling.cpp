#include<bits/stdc++.h>// Importing all standard C++ libraries
typedef long long ll;// Define 'll' as an alias for 'long long'
using namespace std;

// Function to calculate power a^b using binary exponentiation
long long binpow(long long a, long long b) {
    long long res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a;
        a = a * a;
        b >>= 1;
    }
    return res;
}


void write_schedule_to_csv(const vector<vector<ll>>& assign_tasks) {
    ofstream outfile("schedule.csv");
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open file for writing\n";
        return;
    }
    int n=assign_tasks.size();
    int m=assign_tasks[0].size();
    for(int i=1;i<n-1;i++){
        for(int j=1;j<m-1;j++){
            outfile << assign_tasks[i][j] << ((j!=m-2)?",":"");
        }
        outfile << endl;
    }

    outfile.close();
}

// Function to check if it's possible to assign a task to a machine at a given time
ll is_possible(ll M,ll T,ll machine,ll time,unordered_map<ll,ll> & curr_used,vector<vector<ll>> & post_used,vector<vector<ll>>& assign_tasks){

    return post_used[machine][time]+binpow(assign_tasks[machine][time],3)>=binpow(assign_tasks[machine][time]+1,3)+curr_used[machine];
}



// Function to find the parameter used to find the service to be assigned the task in perticular time stamp
ll find_value(ll M,ll T,ll machine,ll time,vector<vector<ll>>& assign_tasks,vector<vector<ll>>& energy_recueved_ini){
    if(energy_recueved_ini[machine][time]<binpow(assign_tasks[machine][time]+1,3)){
        return energy_recueved_ini[machine][time]-binpow(assign_tasks[machine][time]+1,3);
    }
    ll count=cbrt(energy_recueved_ini[machine][time]);
    return -binpow(count,3)+energy_recueved_ini[machine][time];
}

// Function to allot tasks to machines
void allot_tasks(ll M,ll T,ll S,vector<vector<ll>>  energy_received,vector<ll> & tasks,vector<vector<ll>>& assign_tasks){
    vector<vector<ll>> post_used1(M+2,vector<ll> (T+2,INT_MAX));// Initialize post_used matrix with INT_MAX which is used to store weather we can assign task or not at any machine and time slot
    vector<vector<ll>> post_used2(M+2,vector<ll> (T+2,INT_MAX));//stores the parameter used to find the service to be assigned the task in perticular time stamp
    queue<pair<int,int>> nodes1,nodes2;// Initialize two queues for processing tasks one contain slots on which we can schedule tasks in last iteration and other in this

    // Initialize post_used for time slot 0
    for(int i=0;i<M+2;i++){
        post_used1[i][0]=0;
        post_used2[i][0]=0;
    }

    // Calculate energy_received for each time slot
    for(int j=1;j<=T;j++){
        for(int i=1;i<=M;i++){
            energy_received[i][j]+=energy_received[i][j-1];
            nodes1.push({i,j});// Push the edge server and time slot to the queue
        }
    }
    // Calculate post_used for each time slot
    for(int i=T;i>0;i--){
        for(int j=1;j<=M;j++){
            post_used1[j][i]=min(post_used1[j][i+1],energy_received[j][i]+0ll);
            post_used2[j][i]=min(find_value(M,T,i,j,assign_tasks,energy_received),post_used2[j][i+1]);
        }
    }

    // Main loop for task allocation
    while(nodes1.size()!=0){
        unordered_set<int> machines_changed; // Set to keep track of machines whose status changed
        unordered_map<ll,ll> curr_used;// Map to keep track of current power usage
        set<pair<ll,pair<ll,ll>>,greater<pair<ll,pair<ll,ll>>>> tub_ordered;// Ordered set to store machines sorted by available power
        int time_current=nodes1.front().second;// Current time slot being processed

        int check=0;// Flag to check if any task was assigned in this iteration

        // Process all tasks in the current time slot

        while (!nodes1.empty()){
            int machine =nodes1.front().first;
            int time =nodes1.front().second;
            if(time!=time_current){
                // Assign tasks from the ordered set until no more tasks can be assigned
                while ( tub_ordered.size()>0 && tasks[time_current]>0)
                {
                    auto it=tub_ordered.begin();
                    ll machine_current =it->second.first;
                    ll time_current =it->second.second;
                    curr_used[machine_current]+=binpow(assign_tasks[machine_current][time_current]+1,3)-binpow(assign_tasks[machine_current][time_current],3);
                    post_used1[machine_current][time_current]-=curr_used[machine_current];
                    assign_tasks[machine_current][time_current]++;
                    tasks[time_current]--;
                    machines_changed.insert(machine_current);
                    check=1;
                    tub_ordered.erase(tub_ordered.begin());
                }
                tub_ordered.clear();// Clear the ordered set
                time_current=time;// Update the current time slot
                for(auto j:machines_changed){
                    for(int i=T;i>=0;i--){
                        post_used2[j][i]=min(find_value(M,T,i,j,assign_tasks,energy_received),post_used2[j][i+1]);
                    }
                }
            }
            // Check if it's possible to assign a task at the current machine and time
            if(is_possible(M,T,machine,time,curr_used,post_used1,assign_tasks) && tasks[time]>0){
                nodes2.push(nodes1.front());// Push the task to the second queue
                tub_ordered.insert({post_used2[machine][time]-(binpow(assign_tasks[machine][time]+1,3)-assign_tasks[machine][time],3),{machine,time}});// Insert the machine-time pair into the ordered set
            }
            nodes1.pop();// Remove the processed task from the queue
        }

        // Assign tasks from the ordered set until no more tasks can be assigned
        while ( tub_ordered.size()>0 && tasks[time_current]>0)
        {
            auto it=tub_ordered.begin();
            ll machine_current =it->second.first;
            ll time_current =it->second.second;

            curr_used[machine_current]+=binpow(assign_tasks[machine_current][time_current]+1,3)-binpow(assign_tasks[machine_current][time_current],3);
            post_used1[machine_current][time_current]-=curr_used[machine_current];
            assign_tasks[machine_current][time_current]++;
            tasks[time_current]--;
            machines_changed.insert(machine_current);
            check=1;
            tub_ordered.erase(tub_ordered.begin());
        }


        // Break the loop if no task was assigned in this iteration
        if(check==0){
            break;
        }

        // Move tasks from the second queue to the first queue
        while (nodes2.size()>0)
        {
            nodes1.push(nodes2.front());
            nodes2.pop();
        }
        

        // Update post_used for machines whose status changed
        for(auto j:machines_changed){
            for(int i=T;i>=0;i--){
                post_used1[j][i]=min(post_used1[j][i],post_used1[j][i+1]);
            }
        }
    }
}


int main(){
    int M,T,S;
    // Input the number of edge servers, time slots, and upper bound on energy
    cout << "Enter the number of edge servers (M): ";
    cin >> M;
    cout << "\nEnter the number of time slots (T): ";
    cin >> T;
    cout << "\nEnter the upper bound on energy (S_max): ";
    cin >> S;
    
    // Initialize vectors to store energy received, tasks, and assigned tasks
    vector<vector<ll>> energy_received(M+2,vector<ll> (T+2,0));
    vector<ll> tasks(T+2,0);
    vector<vector<ll>> assign_tasks(M+2,vector<ll> (T+2,0));
    
    // Input solar power generation for each edge server and each time slot
    cout << "Enter solar power generation for each edge server and each time slot(MxT):\n" ;
    for(int machine=1;machine<=M;machine++){
        for(int j=1;j<=T;j++){
            int a; cin>>a;
            energy_received[machine][j]=a;
            if(a>S || a<0){
                cout<<"\nWrong input\n";
                return 0;
            }
        }
    }

    // Input the number of tasks arriving at each edge server and each time slot
    cout << "Enter the number of tasks arriving at each edge server and each time slot(MxT):\n";
    for(int machine=1;machine<=M;machine++){
        for(int j=1;j<=T;j++){
            int a; cin>>a;
            tasks[j]+=a;
            if(a<0){
                cout<<"\nWrong input\n";
                return 0;
            }
        }
    }

    // Allot tasks to edge servers
    allot_tasks(M,T,S,energy_received,tasks,assign_tasks);


    // Output the assignment of tasks to machines at different timestamps (MxT)
    cout<<"assignment of task to machines at different time timestamps(M*T):\n";
    for(int i=1;i<=M;i++){
        for(int j=1;j<=T;j++){
            cout<<assign_tasks[i][j]<<" ";
        }
        cout<<"\n";
    }
    write_schedule_to_csv(assign_tasks);
    system("python visualizer.py");
    return 0;
}