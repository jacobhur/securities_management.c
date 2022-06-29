#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>

//Backspace keycode: 8

int day, exchange;

typedef struct _securities{//�ֽ����� structure 
	char code[7];
	char name[50];
	int price;
	int pn;
	int rate;
	struct _securities *next;
}securities;

typedef struct _client_account{//�� ���� �ֽ� structure 
	char securities_code[7];
	int amount;
	struct _client_account *next;
}client_account;

typedef struct _client{//�� strtucture
	char code[5];
	char name[20];
	long long balance;
	struct _client_account *anext;
	struct _client *next;
}client;


securities *head_kr;
securities *head_us;
client *head_client;

void setColor(unsigned short text, unsigned short back) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text|(back<<4));
}

void addFirst(int mode, char code[], char name[], int price, int pn, int rate) {//securities linked list �߰�
	securities *node = malloc(sizeof(securities));
	
	strcpy(node->code, code);
	strcpy(node->name, name);
	node->price = price;
	node->pn=pn;
	node->rate=rate;
	if (mode==0){	
		if (head_kr == NULL) {
			head_kr = node;
			node->next = NULL;
		}
		else {
			node->next = head_kr;
			head_kr = node;
		}
	}
	else{
		if (head_us == NULL) {
			head_us = node;
			node->next = NULL;
		}
		else {
			node->next = head_us;
			head_us = node;
		}
	}
}

void addFirst_client(char code[], char name[], long long balance){//clients linked list �߰�
	client *node=malloc(sizeof(client));
	strcpy(node->code, code);
	strcpy(node->name, name);
	node->balance = balance;
	node->anext=NULL;
	
	if(head_client==NULL){
		head_client=node;
		node->next=NULL;
	}
	else{
		node->next=head_client;
		head_client=node;
	}
}

void addFirst_account(char ccode[], char scode[], int amount){//�� ���� �ֽ� linked list �߰� 
	client_account *atmp, *node=malloc(sizeof(client_account));
	client *tmp;
	
	strcpy(node->securities_code,scode);
	node->amount=amount;
	
	for(tmp=head_client;strcmp(tmp->code,ccode)!=0;tmp=tmp->next);
	if(tmp->anext==NULL){
		tmp->anext=node;
		node->next=NULL;
	}
	else{
		for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
			if(strcmp(scode,atmp->securities_code)==0){
				atmp->amount=atmp->amount+amount;
				return;
			}
		}
		atmp=tmp->anext;
		node->next=atmp;
		tmp->anext=node;
	}
}

void comma(long long amount){//���ڸ����� ��ǥ
	int i=0;
	char cm[30];
	if(amount==0){
		printf("0");
		return;
	}
	while (amount){
		if(i%4!=3){
			cm[i]=48+(amount%10);
			amount=amount/10;
		}
		else
			cm[i]=',';
		i++;
	}
	while(i){
		i--;
		printf("%c",cm[i]);
	}
}

void showall() {//day�� ���� ���� ������ 
	securities *tmp;
	int i;
	if(day%2==0){
		for (tmp = head_kr; tmp != NULL; tmp = tmp->next){
			printf("%s %s  ", tmp->code,tmp->name);
			comma(tmp->price);
			printf("�� ");
			if(tmp->pn==0){
				setColor(12,15);
				printf("�� %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
			else{
				setColor(9,15);
				printf("�� %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
		}
	}
	else{
		for (tmp = head_us; tmp != NULL; tmp = tmp->next){
			printf("%s %s  ", tmp->code,tmp->name);
			comma(tmp->price);
			printf("�޷� ");
			if(tmp->pn==0){
				setColor(12,15);
				printf("�� %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
			else{
				setColor(9,15);
				printf("�� %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
		}
	}
}

securities *code_finder(char code[]){//�ֽ������ڵ�� �ּ�ã�� 
	securities *tmp;
	
	for(tmp=head_kr;tmp!=NULL;tmp=tmp->next){
		if(strcmp(tmp->code,code)==0)
			return tmp;
	}
	
	for(tmp=head_us;tmp!=NULL;tmp=tmp->next){
		if(strcmp(tmp->code,code)==0)
			return tmp;
	}
	
	return tmp;
}

long long total(client *tmp){//���� �ֽ��� ���� ��ġ 
	client_account *atmp;
	long long balance;
	
	balance=tmp->balance;
	if(tmp->anext==NULL)
		return balance;
	for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
		if(isdigit(atmp->securities_code[0])!=0)
			balance=balance+code_finder(atmp->securities_code)->price*atmp->amount;
		else
			balance=balance+code_finder(atmp->securities_code)->price*atmp->amount*exchange;
	}
	return balance;
}

int own_securities(client *ctmp, securities *stmp){//�̹� ������ ������ �ż��� �� ���� ������ �ִ� �ֽ� ������
	client_account *atmp;
	
	if(ctmp->anext==NULL)
		return 0;
	for(atmp=ctmp->anext;atmp!=NULL;atmp=atmp->next){
		if(strcmp(atmp->securities_code,stmp->code)==0)
			return atmp->amount;
	}
}


void showall_client(){//������ ������(���ڵ�, ����, �����ܰ�) 
	client *tmp;
	
	if(head_client==NULL){
		printf("��ϵ� ���� �����ϴ�.");
		return; 
	}
	
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
		printf("%s %s  ",tmp->code,tmp->name);
		comma(tmp->balance);
		printf("�� ");
		comma(total(tmp));
		printf("��\n\n");
	}
}

client *client_finder(char code[]){//���ڵ�� �� �ּ� ã�� 
	client *tmp;
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
		if(strcmp(code,tmp->code)==0){
			return tmp;
		}
	}
	return tmp;
}

void delete_account(client *who,client_account *what){//���� �ֽ� ���� �ŵ��� linked list ���� 
	client_account *tmp;
	if(who->anext==what){
		who->anext=what->next;
	}
	else{
		for(tmp=who->anext;tmp->next!=what;tmp=tmp->next);
		tmp->next=what->next;
	}
	free(what);
}

void existing_code(){//������� ���ڵ� ǥ�� 
	client *tmp;
	
	printf("���� ������� ���ڵ�\n");
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next)
		printf("%s\n",tmp->code);
}

void buy(){//�ż� 
	client *tmp;
	securities *stmp;
	int key,amount,i;
	char code[5], securities_code[7];
	
	showall_client();
	printf("�ż��ϴ� ���� �ڵ带 �Է��ϼ���--->");
	scanf("%s",code);
	
			
	tmp=client_finder(code);
	if(tmp==NULL){
		system("cls");
		printf("�������� �ʴ� ���ڵ� �Դϴ�. �ٽ� Ȯ�����ּ���.\n\n");
		return;
	}
	system("cls");
	showall();
	
	printf("\n\n �ż��� ������ �ڵ带 �Է��ϼ���--->");
	scanf("%s",securities_code);
	for(i=0;i<(int)strlen(securities_code);i++){
		if(securities_code[i]>='a'&&securities_code[i]<='z')
			securities_code[i]=securities_code[i]-32;
	}
	stmp=code_finder(securities_code);
	if(stmp==NULL){
		printf("�������� �ʴ� �����Դϴ�. �ٽ� Ȯ�����ּ���\n\n");
		return;
	}
	system("cls");
	printf("���� %s(%s)������ �������� ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("���Դϴ�. �� �ָ� �ż��ϰڽ��ϱ�?\n%s ",stmp->name);
	comma(stmp->price);
	if(day%2==0)
		printf("��\n---->");
	else{
		printf("�޷�\n\n");
		printf("1USD = %d,%dKRW\n\n---->",exchange/1000,exchange%1000);
	}
	scanf("%d",&amount);
	system("cls");
	if(day%2==0){
		if(tmp->balance<amount*stmp->price){
			printf("�ż������� �ݾ��� �ʰ��Ͽ����ϴ�. �ٽ� Ȯ�����ּ���.\n\n");
			return;
		}
	}
	else{
		if(tmp->balance<amount*stmp->price*exchange){
			printf("�ż������� �ݾ��� �ʰ��Ͽ����ϴ�. �ٽ� Ȯ�����ּ���\n\n");
			return;
		}
	}
	
	if(own_securities(tmp,stmp)>0)
		printf("%s(%s)������ �̹� %d�ָ� �����ϰ� �ֽ��ϴ�. ",tmp->name,tmp->code,own_securities(tmp,stmp));
				
	printf("%s(%s)������ %s %d�� �ż��� �����ϰڽ��ϱ�? �� ",tmp->name,tmp->code,stmp->name,amount);
	if(day%2==0)
		comma(stmp->price*amount);
	else
		comma(stmp->price*amount*exchange);
	printf("���� ��ݵ˴ϴ�.\n1: ��\n�� ��Ű: �ƴϿ�");
	key=getch();
	system("cls");
	if(key!='1'){
		printf("�ż��� ��ҵǾ����ϴ�.\n\n");
		return;
	}
	if(day%2==0)
		tmp->balance=tmp->balance-(stmp->price*amount);
	else
		tmp->balance=tmp->balance-(stmp->price*amount*exchange);
	printf("�ż��ֹ��� �Ϸ�Ǿ����ϴ�. �ܿ� �������� ");
	comma(tmp->balance);
	printf("���Դϴ�.\n\n");
	addFirst_account(tmp->code,stmp->code,amount);
}

void showall_account(){//��� ���� �������� �ֽ� �����ֱ� 
	int tot=0,no=0;
	long long own;
	client *tmp;
	client_account *atmp;
	securities *stmp;
	
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
		own=0;
		tot++;
		if(tmp->anext!=NULL){
			printf("-------------------------\n");
			setColor(9,7);
			printf("%s(%s)������ ������ �ֽ�",tmp->name, tmp->code);
			setColor(7,0);
			printf("\n\n");
			for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
				stmp=code_finder(atmp->securities_code);
				printf("%s %s %d�� ",atmp->securities_code,stmp->name,atmp->amount);
				if(isdigit(stmp->code[0])==0){
					comma(exchange*atmp->amount*stmp->price);
					printf("��\n");
					own+=(stmp->price)*(atmp->amount)*exchange;
				}
				else{
					comma(atmp->amount*stmp->price);
					printf("��\n");
					own+=(stmp->price)*(atmp->amount);
				}
			}
			printf("-------------------------\n");
			setColor(0,7);
			printf("���� ��ġ: ");
			comma(own);
			printf("��");
			setColor(7,0);
			printf("\n\n");
		}
		else
			no++;
	}
	if(tot==no){
		printf("��� ���� ��� �ֽĵ� ������ ���� �ʽ��ϴ�.\n\n");
	}
}

void sell(){//�ŵ� 
	client *tmp;
	client_account *atmp;
	securities *stmp;
	char code[5], securities_code[7];
	int amount,key,p,i;
	showall_account();

	if(day%2==0)
		printf("����� �����̹Ƿ� �ѱ��ֽĸ� �ŷ��� �����մϴ�.\n\n");
	else
		printf("����� �����̹Ƿ� �̱��ֽĸ� �ŷ��� �����մϴ�.\n\n");
	printf("�ŵ��ϴ� ���� �ڵ带 �Է��ϼ���--->");
	scanf("%s",code);
	for(i=0;i<(int)strlen(code);i++){
		if(code[i]>='a'&&code[i]<='z')
			code[i]=code[i]-32;
	}
	tmp=client_finder(code);
	
	if(tmp==NULL){
		system("cls");
		setColor(12,0);
		printf("�������� �ʴ� ���ڵ� �Դϴ�. �ٽ� Ȯ�����ּ���\n\n");
		setColor(7,0);
		return;
	}
	else if(tmp->anext==NULL){
		system("cls");
		setColor(12,0);
		printf("�� ������ �ƹ��� �ֽĵ� ������ ���� �ʽ��ϴ�. �ٽ� Ȯ�����ּ���\n\n");
		setColor(7,0);
		return; 
	}
	system("cls");
	if(day%2==0){
		for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
			if(isdigit(atmp->securities_code[0])!=0)
				p++;
		}
		if(p==0){
			setColor(12,0);
			printf("���� �� ������ �ѱ��ֽ��� ������ ���� �ʽ��ϴ�. �ٽ� Ȯ�����ּ���\n\n");
			setColor(7,0);
			return;
		}
	}
	else{
		for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
			if(isdigit(atmp->securities_code[0])==0)
				p++;
		}
		if(p==0){
			setColor(12,0);
			printf("���� �� ������ �̱��ֽ��� ������ ���� �ʽ��ϴ�. �ٽ� Ȯ�����ּ���\n\n");
			setColor(7,0);
			return;
		}
	}
	if(day%2==0){
		printf("%s(%s)������ ������ �ѱ��ֽ��Դϴ�.\n\n",tmp->name,tmp->code);
			for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
				if(isdigit(atmp->securities_code[0])!=0)
					printf("%s %s %d��\n",atmp->securities_code,code_finder(atmp->securities_code)->name,atmp->amount);
			}
	}
	else{
		printf("%s(%s)������ ������ �̱��ֽ��Դϴ�.\n\n",tmp->name,tmp->code);
			for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
				if(isdigit(atmp->securities_code[0])==0)
					printf("%s %s %d��\n",atmp->securities_code,code_finder(atmp->securities_code)->name,atmp->amount);
			}
	}
	printf("\n��� �ֽ��� �ŵ��ϰڽ��ϱ�? �����ڵ带 �Է����ּ���.---->");
	scanf("%s",securities_code);
	if(day%2==0&&isdigit(securities_code[0])==0){
		system("cls");
		setColor(12,0);
		printf("����� �����̹Ƿ� �ѱ��ֽĸ� �ŷ��� �����մϴ�.\n\n");
		setColor(7,0);
		return; 
	}
	else if(day%2==1&&isdigit(securities_code[0])==1){
		system("cls");
		setColor(12,0);
		printf("����� �����̹Ƿ� �̱��ֽĸ� �ŷ��� �����մϴ�.\n\n");
		setColor(7,0);
		return;
	}
	for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
		if(strcmp(atmp->securities_code,securities_code)==0)
			break;
	}
	system("cls");
	if(atmp==NULL){
		setColor(12,0);
		printf("������ �����ϰ� ���� ���� �ֽ��Դϴ�. �ٽ� Ȯ�����ּ���.\n\n");
		setColor(7,0);
		return;
	}
	printf("%s %d�� �� ���ָ� �ŵ��ϰڽ��ϱ�?--->",code_finder(atmp->securities_code)->name,atmp->amount);
	scanf("%d",&amount);
	system("cls");
	if(atmp->amount<amount){
		setColor(12,0);
		printf("������ �ֽĺ��� ���� �ֽ��� �ŵ��� �� �����ϴ�. �ٽ� Ȯ�����ּ���.\n\n");
		setColor(7,0);
		return;
	}
	printf("%s %d�ָ� �ŵ��Ͻðڽ��ϱ�? �� ",code_finder(atmp->securities_code)->name,amount);
	if(day%2==0)
		comma(amount*code_finder(atmp->securities_code)->price);
	else
		comma(amount*code_finder(atmp->securities_code)->price*exchange);
	printf("���� �Աݵ˴ϴ�.\n1: ��\n�� ��Ű: �ƴϿ�\n");
	key=getch();
	if(key!='1'){
		setColor(12,0);
		printf("�ŵ� �ֹ��� ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	atmp->amount=atmp->amount-amount;
	if(day%2==0){
		for(stmp=head_kr;strcmp(stmp->code,atmp->securities_code)!=0;stmp=stmp->next);
		tmp->balance=tmp->balance+(stmp->price*amount);
	}
	else{
		for(stmp=head_us;strcmp(stmp->code,atmp->securities_code)!=0;stmp=stmp->next);
		tmp->balance=tmp->balance+(stmp->price*amount*exchange);
	}
	system("cls");
	setColor(10,0);
	printf("%s %d�� �ŵ��� �Ϸ�Ǿ����ϴ�. �ܿ� �ִ� %d�� �̸�, �� �������� ",code_finder(atmp->securities_code)->name,amount,atmp->amount);
	comma(tmp->balance);
	printf("���Դϴ�.\n\n");
	setColor(7,0);
	if(atmp->amount==0)
		delete_account(tmp,atmp);
}

void deposit(){//�Ա� 
	char code[5];
	long long balance;
	client *tmp;
	int key;
	
	showall_client();
	printf("�Ա��ϴ� ���� �ڵ带 �Է��ϼ���--->");
	scanf("%s", code);
	tmp=client_finder(code);
	if(tmp==NULL){
		system("cls");
		setColor(12,0);
		printf("���ڵ带 �߸� �Է��߽��ϴ�. �ٽ� Ȯ�����ּ���.\n\n");
		setColor(7,0);
		return;
	}
	system("cls");
	printf("%s(%s)������ �ܰ�� ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("���Դϴ�. �󸶸� �Ա��ϰڽ��ϱ�? \n");
	scanf("%lld",&balance);
	system("cls");
	comma(balance);
	printf("���� �Աݽ� �� �������� ");
	comma(tmp->balance+balance);
	printf("���Դϴ�. �Ա��� �����Ͻðڽ��ϱ�?\n\n1:��\n�� ��Ű:�ƴϿ�");
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("�Ա��� ��ҵǾ����ϴ�.");
		setColor(7,0);
		return;
	} 
	tmp->balance=tmp->balance+balance;
	setColor(10,0);
	printf("�Ա��� �Ϸ�Ǿ����ϴ�.%s(%s)������ �� �������� ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("���Դϴ�.\n\n");
	setColor(7,0);
}

void withdrawl(){//��� 
	int key;
	char code[5];
	client *tmp;
	long long balance;
	
	showall_client();
	printf("����ϴ� ���� �ڵ带 �Է��ϼ���--->");
	scanf("%s",code);
	tmp=client_finder(code);
	if(tmp==NULL){
		system("cls");
		setColor(12,0);
		printf("�������� �ʴ� ���ڵ� �Դϴ�. �ٽ� Ȯ�����ּ���\n\n");
		setColor(7,0);
		return;
	}
	printf("%s(%s)������ �ܰ�� ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("���Դϴ�. �󸶸� ����ϰڽ��ϱ�?\n------>");
	scanf("%lld",&balance);
	system("cls");
	if(tmp->balance<balance){
		setColor(12,0);
		printf("������ ���� ū �ݾ��� ����� �� �����ϴ�. �ٽ� Ȯ�����ֽñ� �ٶ��ϴ�.\n\n");
		setColor(7,0);
		return; 
	}
	comma(balance);
	printf("���� ����ϰڽ��ϱ�? ��ݽ� �ܾ��� ");
	comma(tmp->balance-balance);
	printf("���Դϴ�.\n\n1: ��\n�� ��Ű:�ƴϿ�\n");
	key=getch();
	if(key!='1'){
		setColor(12,0);
		printf("����� ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	tmp->balance=tmp->balance-balance;
	setColor(10,0);
	printf("����� �Ϸ�Ǿ����ϴ�. %s(%s)������ �� �������� ",tmp->name,tmp->code);
	comma(balance);
	printf("���Դϴ�.\n\n");
	setColor(7,0);
}

void new_customer(){//�ű԰� 
	int key;
	char name[20],code[5];
	long long balance;
	client *tmp;
	
	printf("�ű԰� ����� �����Ͻðڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�");
	key=getch();
	if(key!='1'){
		system("cls");
		setColor(12,0);
		printf("�ű԰� ����� ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	system("cls");
	printf("�ű԰��� ������ �Է����ּ���.\n");
	scanf("%s",name);
	
	system("cls");
	existing_code();
	printf("���ϴ� ���ڵ带 �Է��ϼ���. �ڵ�� ���ڸ� ���ڷ� �̷������ �ϰ� �ߺ��� �� �����ϴ�.\n");
	scanf("%s",&code);
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
		if(strcmp(tmp->code,code)==0)
			return;
	}
	while(tmp!=NULL){
		system("cls");
		existing_code();
		setColor(12,0);
		printf("\n\n�ڵ尡 �ߺ��Ǿ����ϴ�. �ٽ� �Է����ּ���.\n");
		setColor(7,0);
		scanf("%s",code);
		for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
			if(strcmp(tmp->code,code)==0)
				return;
		}
	}
	printf("�Ա��� �ݾ��� ���Դϱ�?\n");
	scanf("%lld",&balance);
	
	system("cls");
	printf("%s(%s) ������ ����Ͻðڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�\n",name,code);
	key=getch();
	if(key!='1'){
		setColor(12,0);
		printf("�ű԰� ����� ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	system("cls");
	setColor(10,0);
	printf("%s���� ����� �Ϸ�Ǿ����ϴ�.\n\n",name);
	setColor(7,0);
	addFirst_client(code,name,balance);
}

void remove_customer(){//��Ż�� 
	client *tmp, *tmp2;
	client_account *atmp;
	int key;
	char code[5];
	
	printf("��Ż�� �����ϰڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�\n");
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("��Ż�� ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	showall_client();
	printf("Ż���ϴ� ���� �ڵ带 �Է��ϼ���--->");
	scanf("%s",code);
	system("cls");
	tmp=client_finder(code);
	if(tmp==NULL){
		setColor(12,0);
		printf("�߸��� ���ڵ��Դϴ�. �ٽ� Ȯ�����ּ���");
		setColor(7,0);
		return;
	}
	printf("%s(%s) ������ Ż��ó���ϰڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�\n",tmp->name,tmp->code);
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("��Ż�� ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	if(tmp->anext!=NULL){
		for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
			tmp->anext=atmp->next;
		}
		free(atmp);
	}
	if(tmp==head_client)
		head_client=tmp->next;
	else{
		for(tmp2=head_client;tmp2->next!=tmp;tmp2=tmp2->next);
		tmp2->next=tmp->next;
	}
	free(tmp);
	setColor(10,0);
	printf("��Ż�� �Ϸ�Ǿ����ϴ�.\n\n");
	setColor(7,0);
}

void change_name(){//������ 
	int key;
	client *tmp;
	char code[5], name[20];
	printf("���� ������ �����ϰڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�\n");
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("���� ������ ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	showall_client();
	printf("\n\n���� ������ ������ ���� �ڵ带 �Է����ּ���---->");
	scanf("%s",code);
	system("cls");
	tmp=client_finder(code);
	if(tmp==NULL){
		setColor(12,0);
		printf("�߸��� ���ڵ��Դϴ�. �ٽ� Ȯ�����ּ���.\n\n");
		setColor(7,0);
		return;
	}
	printf("%s(%s)������ ������ �������� �����ϰڽ��ϱ�?--->",tmp->name,tmp->code);
	scanf("%s",name);
	system("cls");
	printf("%s(%s)������ ������ %s�� �����ϰڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�\n",tmp->name,tmp->code,name);
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("���� ������ ��ҵǾ����ϴ�.\n\n");
		setColor(7,0);
		return;
	}
	setColor(10,0);
	printf("%s���� ������ %s�������� ������ �Ϸ�Ǿ����ϴ�.\n\n",tmp->name,name);
	setColor(7,0);
	strcpy(tmp->name,name);
}

void exchange_rate() {//ȯ������
	srand(time(NULL));
	int rate, pn;
	rate = rand() % 40;
	pn = rand() % 2;
	if (pn == 0)
		exchange = exchange + exchange / 1000 * rate;
	else
		exchange = exchange - exchange / 1000 * rate;
	printf("\n\n���� ȯ�� 1USD = %d,%dKRW", exchange/1000, exchange%1000);
}

void homescreen_am() {
	printf("������ %d���� �����Դϴ�. �ѱ��ֽ��� �ŷ��� �� �ֽ��ϴ�.\n\n", day / 2 + 1);
	printf("�����ڵ�  �����    1�ִ� ����  ������񺯵���\n\n");
	showall();
	printf("\n--------------------------------------------------------\n");
	printf("1. ������(����Ÿ�, �����, �ű԰�, ��Ż��)\n");
	printf("2. �������(�űԻ��� �� ����)\n");
	printf("3. �帶��\n");
	printf("0. ���α׷� ����\n");
	printf("--------------------------------------------------------\n");
	printf("��ȣ�� �Է��ϼ���--->");
}

void homescreen_pm() {
	printf("������ %d���� �����Դϴ�. �̱��ֽ��� �ŷ��� �� �ֽ��ϴ�.\n\n", day / 2+1);
	printf("�����ڵ�  �����    1�ִ� ����  ������񺯵���\n\n");
	showall();
	exchange_rate(); 
	printf("\n--------------------------------------------------------\n");
	printf("1. ������(����Ÿ�, �����, �ű԰�, ��Ż��)\n");
	printf("2. �������(�űԻ��� �� ����)\n");
	printf("3. �帶��\n");
	printf("0. ���α׷� ����\n");
	printf("--------------------------------------------------------\n");
	printf("��ȣ�� �Է��ϼ���--->");
}

void price_fluct() {//�ֽİ��� ���� 
	int rate, pn, percent;
	securities *tmp;
	srand(time(NULL));
	if (day%2==0) //�ѱ��ֽ� ���� 
		tmp=head_kr;
	else //�̱��ֽ� ���� 
		tmp=head_us;
		
	while(tmp!=NULL){
		percent=rand()%100;
		if(percent<90){//90%Ȯ���� �ְ� ������ 0~16%�Ͼ�� 10%Ȯ���� �ְ� ������ 17~30%���� �Ͼ 
			rate=rand()%17;
			pn=rand()%2;
			if(pn==0)
				tmp->price=tmp->price + (tmp->price)/100*rate;
			else
				tmp->price=tmp->price - (tmp->price)/100*rate;
		}
		else{
			rate=rand()%14+17;
			pn=rand()%2;
			if(pn==0)
				tmp->price=tmp->price + (tmp->price)/100*rate;
			else
				tmp->price=tmp->price - (tmp->price)/100*rate;
		}
		tmp->pn=pn;
		tmp->rate=rate;
		tmp=tmp->next;
	}
}

void init() {//securities.txt�κ��� linked list ����
	FILE *fp;
	int price, pn, rate;
	char code[7], name[40];
	fp = fopen("securities.txt", "r");
	fscanf(fp,"%d",&day);
	fscanf(fp,"%d",&exchange);
	if (fp == NULL) {
		setColor(12,0);
		printf("�������� �Ǿ� ���� �ʽ��ϴ�.\n");
		setColor(7,0);
		return;
	}
	while (!feof(fp)) {
		fscanf(fp, "%s", code);
		fscanf(fp, "%s", name);
		fscanf(fp, "%d", &price);
		fscanf(fp, "%d", &pn);
		fscanf(fp, "%d", &rate);
		if (isdigit(code[0])!=0){//�����ڵ尡 ���ڷ� �����ϸ� �ѱ�, ����� �����ϸ� �̱�
			addFirst(0, code, name, price,pn,rate);
		}
		else{
			addFirst(1, code, name, price,pn,rate);
		}
	}
	fclose(fp);
}

void init_client(){//������ linked list����
	FILE *fp;
	long long balance;
	char code[5], name[20];
	fp=fopen("clients.txt", "r");
	if (fp == NULL) {
		return;
	}
	while (!feof(fp)) {
		fscanf(fp, "%s", code);
		fscanf(fp, "%s", name);
		fscanf(fp, "%lld", &balance);
		addFirst_client(code, name, balance);
	}
	fclose(fp);
} 

void init_account(){//���� ������ �ֽ� linked list ����
	FILE *fp;
	char client_code[5], securities_code[7];
	int amount;
	fp=fopen("account.txt", "r");
	if (fp == NULL) {
		return;
	}
	while (!feof(fp)) {
		fscanf(fp, "%s", client_code);
		fscanf(fp, "%s", securities_code);
		fscanf(fp, "%d", &amount);
		addFirst_account(client_code,securities_code,amount);
	}
	fclose(fp);	
}

void cmanage(){//������ 
	int key, amount, i, same;
	long long balance;
	char code[5], securities_code[7], name[20];
	client *tmp,*tmp2;
	client_account *atmp,*atmp2;
	securities *stmp;
	srand(time(NULL));
	
	while(1){
		printf("���ڵ� ����  ������  �� �ܰ�\n\n");
		showall_client();
		printf("-----------------------------\n");
		printf("1. �����ֽ� ��ȸ\n");
		printf("2. �ż�\n");
		printf("3. �ŵ�\n");
		printf("4. �Ա�\n");
		printf("5. ���\n");
		printf("6. �ű԰�\n");
		printf("7. ��Ż��\n");
		printf("8. ���� ����\n\n");
		printf("Backspace: ����ȭ������\n"); 
		key=getch();
	
		switch(key){
			case '1':
				system("cls");
				showall_account();
				break;
			
			case '2':
				system("cls");
				buy();
				break;
			
			case '3':
				system("cls");
				sell();
				break;
			
			case '4':
				system("cls");
				deposit();
				break;
			
			case '5': 
				system("cls");
				withdrawl();
				break;
			
			case '6':
				system("cls");
				new_customer();
				break;
			
			case '7':
				system("cls");
				remove_customer();
				break;
		
			case '8':
				system("cls");
				change_name();
				break;
			 
			case 8:
				system("cls");
				return;
			
			default:
				system("cls");
		}
	}
}

void sell_all(securities *node){//��ü ���� �����ϱ�
	client *ctmp;
	client_account *atmp, *atmp2;
	
	for(ctmp=head_client;ctmp!=NULL;ctmp=ctmp->next){
		if(ctmp->anext!=NULL){
			for(atmp=ctmp->anext;atmp!=NULL;atmp=atmp->next){
				if(strcmp(atmp->securities_code,node->code)==0){
					if(day%2==0)
						ctmp->balance=ctmp->balance+(atmp->amount*node->price);
					else
						ctmp->balance=ctmp->balance+(atmp->amount*node->price*exchange);
					
					if(atmp==ctmp->anext)
						ctmp->anext=atmp->next;
					else{
						for(atmp2=ctmp->anext;atmp2->next!=atmp;atmp2=atmp2->next);
						atmp2->next=atmp->next;
					}
					free(atmp);
				}
			}
		}	
	}
}

void smanage(){//�ֽİ��� 
	int key, price, mode,i;
	char code[7], name[40];
	securities *tmp, *smode, *tmp2;
	
	while(1){
		printf("�����ڵ� �����   1�ִ� ����  ������񺯵���\n\n");
		showall();
		printf("------------------------------\n");
		printf("1. �űԻ���\n");
		printf("2. ��������\n\n");
		printf("Backspace: ����ȭ������");
		
		key=getch();
		switch(key){
			case '1'://�űԻ���
				system("cls");
				printf("�űԻ����� �����Ͻðڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�\n");
				key=getch();
				if(key=='1'){;
					system("cls");	
					printf("�űԻ����� �����ڵ带 �Է��ϼ���.\n");
					scanf("%s", code);//���� �����ڵ尡 �ִ��� Ȯ�� 
					for(i=0;i<(int)strlen(code);i++){
						if(code[i]>='a'&&code[i]<='z')
							code[i]=code[i]-32;
					}
					if(day%2==0){
						for(tmp=head_kr;tmp!=NULL;tmp=tmp->next){
							if(strcmp(tmp->code, code)==0)
								break;
						}
					}
					else{
						for(tmp=head_us;tmp!=NULL;tmp=tmp->next){
							if(strcmp(tmp->code, code)==0)
								break;
						}
					}
					if(tmp!=NULL){
						system("cls");
						setColor(12,0);
						printf("�̹� �����ϴ� �����Դϴ�. �ٽ� Ȯ�����ּ���\n\n");
						setColor(7,0);
						break;
					}
				
					printf("\n�űԻ����� �����̸��� �Է��ϼ���.\n");
					scanf("%s", name);
				
					printf("\n�űԻ����� ������ 1�ִ� ������ �Է��ϼ���.\n");
					scanf("%d",&price);
					
					system("cls");
					printf("%s %s ", code, name);
					comma(price);
					if(day%2==0)
						printf("��\n");
					else
						printf("�޷�\n");
						
					printf("\n���� �� ������ �űԻ����� �Ͻðڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�");
					
					key=getch(); 
					
					if(key=='1'){
						system("cls");
						addFirst(day%2, code, name, price,0,0);
						setColor(10,0);
						printf("�űԻ����� �Ϸ�Ǿ����ϴ�.\n\n");
						setColor(7,0);
					}
					else{
						system("cls");
						setColor(12,0);
						printf("�űԻ����� ��ҵǾ����ϴ�.\n\n");
						setColor(7,0);
					}
					free(tmp);
				}
				else
					system("cls");
					
				break;
				
			case'2'://��������, ���������� �� ���� �ش� �ָ� ������ ������ ���� �ŵ� ó��
				system("cls");
				printf("���������� �����Ͻðڽ��ϱ�?\n1: ��\n�� ��Ű: �ƴϿ�");
				key=getch();
				
				if(key!='1'){
					system("cls");
					setColor(12,0);
					printf("���������� ��ҵǾ����ϴ�.\n\n");
					setColor(7,0);
					break;
				}
				system("cls");
				showall();
				printf("\n--------------------------------------\n");
				printf("���������� �����ڵ带 �Է��ϼ���.\n");
				scanf("%s", code);
				system("cls");
				//��ġ�ϴ� ���� ã�� ������ ���� ���
				if(day%2==0){
					for(tmp=head_kr;tmp!=NULL;tmp=tmp->next){
						if(strcmp(tmp->code, code)==0)
							break;
					}
				}
				else{
					for(tmp=head_us;tmp!=NULL;tmp=tmp->next){
						if(strcmp(tmp->code, code)==0)
							break;
					}
				}
				if(tmp==NULL){
					setColor(12,0);
					printf("�������� �ʴ� �ڵ��Դϴ�. �ٽ� Ȯ�����ּ���.\n\n");
					setColor(7,0);
					break;
				}
				system("cls");
				printf("%s %s\n",tmp->code, tmp->name);
				printf("������ �� ������ ���������Ͻðڽ��ϱ�? �� ������ �ֽ��� ������ ���� ��� �ܷ��� �ŵ��˴ϴ�.\n\n1: ��\n�� ��Ű: �ƴϿ�");
				
				key=getch();
					
				if(key!='1'){
					system("cls");
					setColor(12,0);
					printf("���������� ��ҵǾ����ϴ�.\n\n");
					setColor(7,0);
					break;
				}
				sell_all(tmp);
				
				if(day%2==0){
					if(tmp==head_kr){
						head_kr=tmp->next;
					}
					else{
						for(tmp2=head_kr;tmp2->next!=tmp;tmp2=tmp2->next);
						tmp2->next=tmp->next;
					}
				}
				else{
					if(tmp==head_us){
						head_us=tmp->next;
					}
					else{
						for(tmp2=head_us;tmp2->next!=tmp;tmp2=tmp2->next);
						tmp2->next=tmp->next;
					}
				}
				free(tmp);
				system("cls");
				setColor(10,0);
				printf("���������� �Ϸ�Ǿ����ϴ�.\n\n");
				setColor(7,0);
				break;
			
			case 8:
				system("cls");
				return;
			
			default:
				system("cls");
		}
	}
}

//�ؽ�Ʈ ���� ��� 
void terminate(){
	FILE *fp;
	securities *stmp;
	client *ctmp;
	client_account *atmp;
	
	fp=fopen("securities.txt","w");
	fprintf(fp,"%d\n%d",day,exchange);
	for(stmp=head_kr;stmp!=NULL;stmp=stmp->next)
		fprintf(fp,"\n%s %s %d %d %d",stmp->code,stmp->name,stmp->price,stmp->pn,stmp->rate);
	for(stmp=head_us;stmp!=NULL;stmp=stmp->next)
		fprintf(fp,"\n%s %s %d %d %d",stmp->code,stmp->name,stmp->price,stmp->pn,stmp->rate);
	fclose(fp);
	
	fp=fopen("clients.txt","w");
	for(ctmp=head_client;ctmp!=NULL;ctmp=ctmp->next)
		fprintf(fp,"\n%s %s %lld",ctmp->code,ctmp->name,ctmp->balance);
	fclose(fp);
	
	fp=fopen("account.txt","w");
	for(ctmp=head_client;ctmp!=NULL;ctmp=ctmp->next){
		if(ctmp->anext!=NULL){
			for(atmp=ctmp->anext;atmp!=NULL;atmp=atmp->next)
				fprintf(fp, "\n%s %s %d",ctmp->code,atmp->securities_code,atmp->amount);
		}
	}
	fclose(fp);
}


int main() {
	int key;
	init(); //���� ����Ʈ ���� �� �ѱ��� �̱� �����Ͽ� ����, �ڵ尡 ���ڷ� �����ϸ� �ѱ�, ����� �����ϸ� �̱�
	init_client();//������ ����Ʈ ����
	init_account();//�����ֽļ� ����Ʈ ����
	setColor(11,14);
	printf("Welcome to JEONGWON's Securities");
	setColor(7,0);
	printf("\n--------------------------------------------\n");
	
	while (1) {
		key=0;
		while(key!='3'){
			if (day % 2 == 0)
				homescreen_am();
			else
				homescreen_pm();
			
			key = getch();
			switch (key) {
				case '1'://������
					system("cls");
					cmanage();
					break;
				case'2'://�������
					system("cls");
					smanage();
					break;
				case'3':
					system("cls");
					printf("���� ���� �����ϰڽ��ϱ�?\n3: ��\n�� ��Ű: �ƴϿ�\n");
					key=getch();
					if(key!='3'){
						system("cls");
						break;
					}
					price_fluct();
					system("cls");
					break;
				case'0':
					system("cls");
					setColor(12,0);
					printf("������ ���α׷��� �����ϰڽ��ϱ�?");
					setColor(7,0);
					printf("\n\n1: ��\n�� ��Ű: �ƴϿ�");
					key=getch();
					if(key=='1'){
						system("cls");
						printf("Bye Bye");
						terminate();//��� ��Ȳ �ؽ�Ʈ���Ͽ� ����
						exit(1);
					}
					system("cls");
					
				default:
					system("cls");
			}
		}
		day++;
	}
}
