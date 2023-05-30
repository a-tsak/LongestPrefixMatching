#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef int treeItem;

typedef struct treenode *treelink;

struct treenode{
	
	treeItem data;
	treelink left, right;
	
};

treelink createleaf(treeItem data); // Creates a node

struct treenode* createtree(struct treenode* root, long p[], int a[], int *gate);  // Creates subtrees with prefixes
struct treenode* tobinaryfromfile(FILE *fp, struct treenode* root, long p[], int a[]);  // Converts dotted decimal IP subnets to binary prefixes
void tobinaryfromkeyboard(struct treenode* root, long p[], int a[]);    // Converts user-input decimal IP address to binary

struct treenode* insert(struct treenode* node, int a[], int N, int i, int j, int *gate);       // Traverses tree to insert a node (and then calls createleaf to create it)
struct treenode* match(struct treenode* node, int a[], int N, int i, int *j, int *sameflag);    // Traverses tree (based on array) to find if user-input IP address matches with any prefix
struct treenode* longestprefixmatching(struct treenode* node, int a[], int N, int i, int *j);   // Traverses tree (based on array) to find which prefix the IP matched is the longest

int main(){
	
	treelink root = NULL;       // Pointer of the root will point to NULL at first
	FILE *fp;
	long int p[5], q[4];        // p[5] = parts of dotted decimal IP subnet a.b.c.d/e
    int a[32];                  // a[32] binary representation of the IP subnet
    
    fp = fopen("routingtable.txt","r");
    
    if (fp == NULL) {
        printf("Error: file pointer is null.");
        exit(1);
    }
    
    root = tobinaryfromfile(fp,root,p,a);
    // Reads all IP subnets from the file
    // Converts decimal digits from fields to binary digits, to make the prefix
    // Creates a subtree based on the prefix
    
    fclose(fp);
    
	printf("Please enter an IP address.\n");
	
    tobinaryfromkeyboard(root,q,a);
    // Reads an IP address from the user
    // Checks if any prefix matches with the IP address
    // Checks which is the longest prefix matching
}

struct treenode* createtree(struct treenode* root, long p[], int a[], int *gate){  // o = Number of gate
    
    int i=0, num;       // i = Level of the current node
	treelink tmproot = root;
	
	while(i<p[4]+1){	// p[4] = number of constant binary digits, (e from a.b.c.d/e)
		
		num = a[i];
		
		if (i==0 && root!=NULL){
		    num=0;
		    root->data=num;
		}
		
		if (root == NULL){      // If root doesn't exist, create the root node
			
			if (i==0){
				num=0;
			}
			root = createleaf(num);
			
		}
		
		else{           // If the root exists, go to its child node
		    
		    tmproot = root;
		    tmproot = insert(tmproot, a, p[4], 0, i-1, gate);        // i = 0 (traverses the a[] array) , j = i-1, gate = number of gate
		    // Insert will be called for every level
		}

		i++;
	}
	
	*gate=*gate+1;            // Number of gate
	
	return root;
}

struct treenode* tobinaryfromfile(FILE *fp, struct treenode* root, long p[], int a[]){
    
    char *str, *dup, *split[5], *token;
    char c, *d;
    int k, i, j, temp[5], *gate, b;        // *gate = pointer to the number of gate
    
    str = (char*)malloc(255*sizeof(char));
    
    gate=&b;       // gate will point to b
    *gate=1;       // Number of gate, starts from 1 and finishes in 198
    
    d = &c;     // Needed for strtol()
    
    while(fgets(str, 255, fp)) {        // Reads file line by line
        i=0;
        
        char *dup = strdup(str);        // Duplicates string "str" to "dup"
        
        token= strtok(dup, "/");        // token = a.b.c.d
        
        token= strtok(NULL, "/");       // token = e
        split[4] = token;
        
        token = strtok(dup, ".");       // token = a (i==0)
        while (token != NULL)
        {
            split[i] = token;
            token = strtok(NULL, ".");  // token = b (i==1), token = c (i==2), token = d (i==3)
            i++;
        }
        
        for (j=0; j<5; j++){
            p[j] = strtol(split[j], &d, 10);    // Make each string part (split[5]) into a long int (p[5])
        }
        free(dup);      // Free duplicated string
        
    	int j=0;
    	
    	for (i=0; i<32; i++){   // Initialize all array elements to 0
    	    a[i]=0;     // a[32] = binary representation of the 32-bit IP address
    	}
    	
    	for (i=0; i<4; i++){
    	    temp[i]=p[i];   // temp[4] = fields in decimal form (a,b,c,d)
    	}
    	
    	for (k=0; k<4; k++){    // For every field
            
            i=((k+1)*8)-1;      // i=position of a[] array, k = 0: i = 7 to 0, k = 1: i = 15 to 8, k = 2: i = 23 to 16, k=3: i = 31 to 24
            j++;
            
            while(temp[k]>0 && j<p[4]){
                
                if (temp[k]%2==0){  // If the field number is even, its last binary digit will be 0
        	        a[i]=0;
        	    }
        	    
        	    else{               // If the field number is odd, its last binary digit will be 1
        	        a[i]=1;
        	    }
        	    
        	    i--;        // i will decrease since we calculate the remaining last binary digit in every while loop
        	    
        	    temp[k]=temp[k]/2;  // Field number gets divided by 2
            }
        }
        
    	root = createtree(root,p,a,gate);  // Create a subtree and traverse based on the a[32] array
    }
    
	return root;
    
}

void tobinaryfromkeyboard(struct treenode* root, long p[], int a[]){
    
    int c, i, j, k, l, temp[4], *ptr, *sameflag, flag,count;        // Sameflag will be 1 when the IP matched with a prefix
    // If there is a prefix that starts with the prefix that matched (so the number of the gate is in the node data), but in the next digits it doesn't match, it is 0
	ptr=&c;                                                         // For example 192.168.17.100 will match with 192.168.17.0/24 and 192.168.16.0/20, sameflag will be 1 for both prefixes
    sameflag=&flag;                                                     // 94.64.50.100 will match with 94.64.0.0/13 but not 94.64.128.0/17, it will be 1 for the first prefix
    
    char *str, *end;
    str = (char*)malloc(255*sizeof(char));
    
    do{
        fgets(str, 255, stdin);        // Reads file line by line
        
        end = str;
        i=0;
        while(*end && i<4) {               // For every field
        	long int n = strtol(str, &end, 10); // Make each string field into long int field n
        	
        	p[i]=n;
        	
        	i++;
        	while (*end == '.') {           // If '.' is the next character, skip this character
        		end++;
        	}
        	str = end;                      // Make str point to where end points
        	
        }
        count=0;
        for (k=0; k<4; k++){
            if (p[k]>=0 && p[k]<=255){      // If every field is a valid number [0,255]
                count++;
            }
        }
        if (count<4){       // Count should be 4 because of the 4 fields
            printf("Please try again! All fields must range from 0 to 255.\n");
        }
        
    }while(count!=4);       // Checks if IP address is valid, (a,b,c,d = [0,255])
	
	for (i=0; i<32; i++){   // Initialize all array elements to 0
		a[i]=0;     // a[32] = binary representation of the 32-bit IP address
	}
	for (i=0; i<4; i++){
		temp[i]=p[i];   // temp[4] = fields in decimal form (a,b,c,d)
	}
	
	j = 0;                      // j=counter for p[4] value, which is the number of digits who will stay constant
	for (k=0; k<4; k++){        // For every field
        
		i=((k+1)*8)-1;      // i=position of a[] array, k = 0: i = 7 to 0, k = 1: i = 15 to 8, k = 2: i = 23 to 16, k=3: i = 31 to 24
        j++;
        
        while(temp[k]>0 && j<p[4]){
            
            if (temp[k]%2==0){  // If the field number is even, its last binary digit will be 0
				a[i]=0;
			}
    	    else{               // If the field number is odd, its last binary digit will be 1
    	        a[i]=1;
    	    }
    	    
    	    i--;        // i will decrease since we calculate the remaining last binary digit in every while loop
    	    
    	    temp[k]=temp[k]/2;  // Field number gets divided by 2
        }
    
	}
    
	*sameflag = 0;
	root = match(root, a, 32, 0, ptr, sameflag);
	
	if ( (*ptr == 0) && ((*sameflag)==0)){          // The IP did not match with any prefix
	    printf("IP matched with the root.");            // It matches with the root's subnet 0.0.0.0/0 and the data will go to that gate
	}
	
	*ptr = 0;
	root = longestprefixmatching(root, a, 32, 0, ptr);
	
	if (*ptr != -1){
	    printf("\nLongest prefix matching: ");
	    for(l=0; l<(*ptr); l++){
	        printf("%d", a[l]);
	    }
	    printf("\n");
	}
	
}

struct treenode* insert(struct treenode* node, int a[], int N, int i, int j, int *gate)
{

    if( node == NULL ){             // If the node doesn't exist, make a node
        node = createleaf(0);
	}
	
	if (i<=j){              // Traversing every tree level until j level is reached (For example 10th level of a 25-bit prefix)
	            
        if( a[i] == 1 ){
            node->right = insert(node->right, a, N, i+1, j, gate);     // Going to the right node, i++ because i is the number of the level
        }
        else{
            node->left = insert(node->left, a, N, i+1, j, gate);     // Going to the right node, i++ because i is the number of the level
        } 
        
    }
    
    if (i==N){              // If the current level number is the last one the prefix has, the data will contain the number of the gate
    	node->data = *gate;
	}
	
    return node;
    
}

struct treenode* match(struct treenode* node, int a[], int N, int i, int *j, int *sameflag)
{
	int l;
    if (node==NULL){
        if (*j != 1){       // If the IP has not already matched with a prefix
            *j = 0;
        }
	    return node;
	}
	
    if( node->data >= 1){       // If node is a gate, the IP matched with a prefix
        
		*j = 1; 
        
        l=0;
        
        if (l<i){
		    printf("\nPrefix matched: ");
		    *sameflag=1;        // If IP matched with a prefix, sameflag will be 1
        }
        
        for (l=0; l<i; l++){    // Print the prefix
            printf("%d", a[l]);
        }
        
    }
    else{           // If node is not a gate
        *j=0;
    }
    
	if (i<N){           // If (i<p[4]) == If current level is not the last level of the tree
        
        if( a[i] == 1 ){
            node->right = match(node->right, a, N, i+1, j, sameflag);     // Going to the right node, i++ because i is the number of the level
        }
        else {
            node->left = match(node->left, a, N, i+1, j, sameflag);     // Going to the right node, i++ because i is the number of the level
        } 
        
    }
    
	if (i==32){     // If current level is the last level of the tree
        return node;
	}
	
}

struct treenode* longestprefixmatching(struct treenode* node, int a[], int N, int i, int *j)
{
    
	if (node==NULL){
        if (*j == 0){       // If the IP has not already matched with a prefix
            *j = -1;
        }
	    return node;
	}
	
    if( node->data >= 1 ){       // If node is a gate, the IP matched with a prefix
        *j = i;                     // j will point to the number of the level the gate is
    }
    
	if (i<N){               // If (i<p[4]) == If current level is not the last level of the tree
        if( a[i] == 1 ){
            node->right = longestprefixmatching(node->right, a, N, i+1, j);     // Going to the right node, i++ because i is the number of the level
        }
        else {
            node->left = longestprefixmatching(node->left, a, N, i+1, j);     // Going to the right node, i++ because i is the number of the level
        } 
    }
    
	if (i==32){     // If current level is the last level of the tree
        return node;
	}
	
}

treelink createleaf(treeItem data){
	
	treelink tmproot = (treelink)malloc(sizeof (tmproot));
	tmproot->data = data;
	tmproot->left = tmproot->right = NULL;
}
