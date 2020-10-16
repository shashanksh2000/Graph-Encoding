#include<iostream>
#include<string>
#include<cmath>
#include<fstream>
using namespace std;

class Node {
    public:
        int data;
        Node* next;
};

class Buffer{
    public:
        char key[26] = {NULL};
        Node* head[26] = {NULL};
        int index(char c) {
            for(int i = 0; i < 26; i++) {
                if(c == key[i]) {
                    return i;
                }
            }
            return -1;
        }
};

Buffer buf;

void encodeMsgMenu() {
    cout << "\nHow would you like to encode the message? \n1. From existing file \n2. From user \n3. Exit\n";
    cout << "\nEnter your choice: ";
    return;
}

Node* encodeMatrix(char c) {
    int ascii = c, binary_32 = 32;
    int xorval = ascii^binary_32, k = 0;
    int temp = xorval;
    while(temp) {
        k += temp & 1;
        temp >>= 1;
    }
    int n = log2(xorval);
    int binary[n], pos = n - 1;
    while(xorval){
        binary[pos] = xorval%2;
		xorval/= 2;
        pos--;
	}
    pos = 0;
    int adjList[k], adjMatrix[k][k];
    for(int i = 0; i < k; i++) {
        int count = 1;
        while(pos < n && binary[pos] != 1) {
            pos++;
            count++;
        }
        adjList[i] = count;
        pos++;
    }

    for(int i = 0; i < k; i++) {
        for(int j = 0; j < k; j++) {
            adjMatrix[i][j] = 0;
        }
    }

    for(int i = 0; i < k; i++) {
        adjMatrix[i][(i + 1)%k] = adjList[i];
        adjMatrix[(i + 1)%k][i] = adjList[i];
    }
    
    Node* head = new Node();
    Node* curr = new Node();
    curr->data = k;
    curr->next = NULL;
    head = curr;
    for(int i = 0; i < k; i++) {
        for(int j = 0; j < k; j++) {
            Node* temp = new Node();
            temp->data = adjMatrix[i][j];
            curr->next = temp;
            curr = temp;
            temp->next = NULL;
        }
    }
    return head;
}

void encodeBuffer(string s, string file) {
    fstream output;
    file += ".txt";
    output.open(file, ios::out | ios::trunc);
    for(int i = 0; i < s.length(); i++) {
        if(s[i] == ' ') {
            output << "$\n";
            continue;
        }
        int flag = 1;
        if(s[i] >= 97 && s[i] <= 122) {
            s[i] = s[i] - 'a' + 'A';
            flag = 0;
        }
        int pos = buf.index(s[i]);
        if(pos != -1) {
            //output the linked list in the file.
            Node* ptr = buf.head[pos];
            while(ptr != NULL) {
                output << ptr->data; //<< " ";
                ptr = ptr->next;
            }
            output << flag;
            output << "\n";
        }
        else {
            pos = s[i] - 'A';
            buf.key[pos] = s[i];
            buf.head[pos] = encodeMatrix(s[i]);
            Node* ptr = buf.head[pos];
            while(ptr != NULL) {
                output << ptr->data;// << " ";
                ptr = ptr->next;
            }
            output << flag;
            output << "\n";
        }
    }
    output.close();
    return;
}

void fileEncoding() {
    string msgFile, decodeFile, msg;
    cout << "\nEnter the name of the file which contains the msg for encoding: ";
    cin >> msgFile;
    cout << "\nEnter the name of the destination file for transmission of encoded file: ";
    cin >> decodeFile;
    msgFile += ".txt";
    fstream inputFile;
    inputFile.open(msgFile);
    if(!inputFile.is_open()) {
        cout << "Error in opening file.";
        return;
    }
    if(inputFile.bad()) {
        cout << "Error in reading the file";
        return;
    }
    getline(inputFile, msg);
    inputFile.close();
    encodeBuffer(msg, decodeFile);
    return;
}

void userEncoding() {
    string msg, decodeFile;
    cout << "\nEnter the secret message to encode: ";
    cin.ignore();
    getline(cin, msg);
    cout << "\nEnter the name of the destination file for transmission of encoded file: ";
    cin >> decodeFile;
    encodeBuffer(msg, decodeFile);
    return;
}

void encode() {
    int type;
    encodeMsgMenu();
    cin >> type;
    while(type != 3) {
        switch(type) {
            case 1: fileEncoding();
                    type = 3;
                    break;
            case 2: userEncoding();
                    type = 3;
                    break;
            default:cout << "Please enter a valid choice. \n";
                    encodeMsgMenu();
                    cin >> type;
        }
    }
    cout << "\nMessage encoded successfully.\n";
}

void decode() {
    string decodeFile, msg;
    cout << "\nEnter the name of file containing encoded message: ";
    cin >> decodeFile;

    decodeFile += ".txt";
    fstream inputFile;
    inputFile.open(decodeFile);

    if(!inputFile.is_open()) {
        cout << "Error in opening file.";
        return;
    }
    if(inputFile.bad()) {
        cout << "Error in reading the file";
        return;
    }
    cout << "\nSecret Message is: ";
    while(getline(inputFile, msg)) {
        if(msg[0] == '$') {
            cout << " ";
            continue;
        }
        int size = msg[0] - '0';
        int adjMatrix[size][size], j = 0, k = 0, count = 0, adjList[size], binary[7], pos = 0, xorval = 0, ascii, flag;
        for(int i = 1; i < msg.length(); i++) {
            if(i == msg.length() - 1) {
                flag = msg[i] - '0';
                continue;
            }
            if(msg[i] == ' ') {
                continue;
            }
            if(count%size == 0 && count) {
                count = 0;
                j++;
                k = 0;
            }
            adjMatrix[j][k] = msg[i] - '0';
            count++;
            k++;
        }
        for(int i = 0; i < size; i++) {
            adjList[i] = adjMatrix[i][(i + 1)%size];
            binary[pos] = 1;
            pos++;
            for(int j = 0; j < adjList[i] - 1; j++) {
                binary[pos] = 0;
                pos++;
            }
        }
        for(int i = 6; i >= 0; i--) {
            xorval += pow(2, (6 - i)) * binary[i];
        }
        ascii = xorval^32;
        char c = ascii;
        if(flag == 0) {
            c = c - 'A' + 'a';
        }
        cout << c;
    }
    inputFile.close();
    cout << "\n\nMessage decoded successfully.\n";
    return;
}

void mainMsgMenu() {
    cout << "\nChoose from the following menu: \n1. Encode \n2. Decode \n3. Exit\n";
    cout << "\nEnter your choice: ";
}

void mainMenu() {
    int choice;
    bool check = true;
    mainMsgMenu();
    cin >> choice;
    while(choice != 3) {
        switch(choice) {
            case 1: encode();
                    check = true;
                    break;
            case 2: decode();
                    check = true;
                    break;
            default:cout << "Please enter a valid choice. \n";
                    mainMsgMenu();
                    cin >> choice;
                    check = false;
        }
        if(check) {
            mainMsgMenu();
            cin >> choice;
        }
    }
}

int main() {
    mainMenu();
    return 0;
}