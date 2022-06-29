#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>

//Backspace keycode: 8

int day, exchange;

typedef struct _securities{//주식종목 structure 
	char code[7];
	char name[50];
	int price;
	int pn;
	int rate;
	struct _securities *next;
}securities;

typedef struct _client_account{//고객 보유 주식 structure 
	char securities_code[7];
	int amount;
	struct _client_account *next;
}client_account;

typedef struct _client{//고객 strtucture
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

void addFirst(int mode, char code[], char name[], int price, int pn, int rate) {//securities linked list 추가
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

void addFirst_client(char code[], char name[], long long balance){//clients linked list 추가
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

void addFirst_account(char ccode[], char scode[], int amount){//고객 보유 주식 linked list 추가 
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

void comma(long long amount){//세자리마다 쉼표
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

void showall() {//day에 따라서 종목 보여줌 
	securities *tmp;
	int i;
	if(day%2==0){
		for (tmp = head_kr; tmp != NULL; tmp = tmp->next){
			printf("%s %s  ", tmp->code,tmp->name);
			comma(tmp->price);
			printf("원 ");
			if(tmp->pn==0){
				setColor(12,15);
				printf("▲ %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
			else{
				setColor(9,15);
				printf("▼ %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
		}
	}
	else{
		for (tmp = head_us; tmp != NULL; tmp = tmp->next){
			printf("%s %s  ", tmp->code,tmp->name);
			comma(tmp->price);
			printf("달러 ");
			if(tmp->pn==0){
				setColor(12,15);
				printf("▲ %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
			else{
				setColor(9,15);
				printf("▼ %d%% ",tmp->rate);
				setColor(7,0);
				printf("\n\n");
			}
		}
	}
}

securities *code_finder(char code[]){//주식종목코드로 주소찾기 
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

long long total(client *tmp){//보유 주식의 현재 가치 
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

int own_securities(client *ctmp, securities *stmp){//이미 보유한 종목을 매수할 때 현재 가지고 있는 주식 보여줌
	client_account *atmp;
	
	if(ctmp->anext==NULL)
		return 0;
	for(atmp=ctmp->anext;atmp!=NULL;atmp=atmp->next){
		if(strcmp(atmp->securities_code,stmp->code)==0)
			return atmp->amount;
	}
}


void showall_client(){//고객정보 보여줌(고객코드, 고객명, 보유잔고) 
	client *tmp;
	
	if(head_client==NULL){
		printf("등록된 고객이 없습니다.");
		return; 
	}
	
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
		printf("%s %s  ",tmp->code,tmp->name);
		comma(tmp->balance);
		printf("원 ");
		comma(total(tmp));
		printf("원\n\n");
	}
}

client *client_finder(char code[]){//고객코드로 고객 주소 찾기 
	client *tmp;
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
		if(strcmp(code,tmp->code)==0){
			return tmp;
		}
	}
	return tmp;
}

void delete_account(client *who,client_account *what){//보유 주식 전량 매도시 linked list 삭제 
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

void existing_code(){//사용중인 고객코드 표시 
	client *tmp;
	
	printf("현재 사용중인 고객코드\n");
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next)
		printf("%s\n",tmp->code);
}

void buy(){//매수 
	client *tmp;
	securities *stmp;
	int key,amount,i;
	char code[5], securities_code[7];
	
	showall_client();
	printf("매수하는 고객의 코드를 입력하세요--->");
	scanf("%s",code);
	
			
	tmp=client_finder(code);
	if(tmp==NULL){
		system("cls");
		printf("존재하지 않는 고객코드 입니다. 다시 확인해주세요.\n\n");
		return;
	}
	system("cls");
	showall();
	
	printf("\n\n 매수할 종목의 코드를 입력하세요--->");
	scanf("%s",securities_code);
	for(i=0;i<(int)strlen(securities_code);i++){
		if(securities_code[i]>='a'&&securities_code[i]<='z')
			securities_code[i]=securities_code[i]-32;
	}
	stmp=code_finder(securities_code);
	if(stmp==NULL){
		printf("존재하지 않는 종목입니다. 다시 확인해주세요\n\n");
		return;
	}
	system("cls");
	printf("현재 %s(%s)고객님의 예수금은 ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("원입니다. 몇 주를 매수하겠습니까?\n%s ",stmp->name);
	comma(stmp->price);
	if(day%2==0)
		printf("원\n---->");
	else{
		printf("달러\n\n");
		printf("1USD = %d,%dKRW\n\n---->",exchange/1000,exchange%1000);
	}
	scanf("%d",&amount);
	system("cls");
	if(day%2==0){
		if(tmp->balance<amount*stmp->price){
			printf("매수가능한 금액을 초과하였습니다. 다시 확인해주세요.\n\n");
			return;
		}
	}
	else{
		if(tmp->balance<amount*stmp->price*exchange){
			printf("매수가능한 금액을 초과하였습니다. 다시 확인해주세요\n\n");
			return;
		}
	}
	
	if(own_securities(tmp,stmp)>0)
		printf("%s(%s)고객님은 이미 %d주를 보유하고 있습니다. ",tmp->name,tmp->code,own_securities(tmp,stmp));
				
	printf("%s(%s)고객님의 %s %d주 매수를 진행하겠습니까? 총 ",tmp->name,tmp->code,stmp->name,amount);
	if(day%2==0)
		comma(stmp->price*amount);
	else
		comma(stmp->price*amount*exchange);
	printf("원이 출금됩니다.\n1: 예\n그 외키: 아니요");
	key=getch();
	system("cls");
	if(key!='1'){
		printf("매수가 취소되었습니다.\n\n");
		return;
	}
	if(day%2==0)
		tmp->balance=tmp->balance-(stmp->price*amount);
	else
		tmp->balance=tmp->balance-(stmp->price*amount*exchange);
	printf("매수주문이 완료되었습니다. 잔여 예수금은 ");
	comma(tmp->balance);
	printf("원입니다.\n\n");
	addFirst_account(tmp->code,stmp->code,amount);
}

void showall_account(){//모든 고객이 보유중인 주식 보여주기 
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
			printf("%s(%s)고객님이 보유한 주식",tmp->name, tmp->code);
			setColor(7,0);
			printf("\n\n");
			for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
				stmp=code_finder(atmp->securities_code);
				printf("%s %s %d주 ",atmp->securities_code,stmp->name,atmp->amount);
				if(isdigit(stmp->code[0])==0){
					comma(exchange*atmp->amount*stmp->price);
					printf("원\n");
					own+=(stmp->price)*(atmp->amount)*exchange;
				}
				else{
					comma(atmp->amount*stmp->price);
					printf("원\n");
					own+=(stmp->price)*(atmp->amount);
				}
			}
			printf("-------------------------\n");
			setColor(0,7);
			printf("현재 가치: ");
			comma(own);
			printf("원");
			setColor(7,0);
			printf("\n\n");
		}
		else
			no++;
	}
	if(tot==no){
		printf("모든 고객이 어떠한 주식도 가지고 있지 않습니다.\n\n");
	}
}

void sell(){//매도 
	client *tmp;
	client_account *atmp;
	securities *stmp;
	char code[5], securities_code[7];
	int amount,key,p,i;
	showall_account();

	if(day%2==0)
		printf("현재는 오전이므로 한국주식만 거래가 가능합니다.\n\n");
	else
		printf("현재는 오후이므로 미국주식만 거래가 가능합니다.\n\n");
	printf("매도하는 고객의 코드를 입력하세요--->");
	scanf("%s",code);
	for(i=0;i<(int)strlen(code);i++){
		if(code[i]>='a'&&code[i]<='z')
			code[i]=code[i]-32;
	}
	tmp=client_finder(code);
	
	if(tmp==NULL){
		system("cls");
		setColor(12,0);
		printf("존재하지 않는 고객코드 입니다. 다시 확인해주세요\n\n");
		setColor(7,0);
		return;
	}
	else if(tmp->anext==NULL){
		system("cls");
		setColor(12,0);
		printf("위 고객님은 아무런 주식도 가지고 있지 않습니다. 다시 확인해주세요\n\n");
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
			printf("현재 위 고객님은 한국주식을 가지고 있지 않습니다. 다시 확인해주세요\n\n");
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
			printf("현재 위 고객님은 미국주식을 가지고 있지 않습니다. 다시 확인해주세요\n\n");
			setColor(7,0);
			return;
		}
	}
	if(day%2==0){
		printf("%s(%s)고객님이 보유한 한국주식입니다.\n\n",tmp->name,tmp->code);
			for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
				if(isdigit(atmp->securities_code[0])!=0)
					printf("%s %s %d주\n",atmp->securities_code,code_finder(atmp->securities_code)->name,atmp->amount);
			}
	}
	else{
		printf("%s(%s)고객님이 보유한 미국주식입니다.\n\n",tmp->name,tmp->code);
			for(atmp=tmp->anext;atmp!=NULL;atmp=atmp->next){
				if(isdigit(atmp->securities_code[0])==0)
					printf("%s %s %d주\n",atmp->securities_code,code_finder(atmp->securities_code)->name,atmp->amount);
			}
	}
	printf("\n어느 주식을 매도하겠습니까? 종목코드를 입력해주세요.---->");
	scanf("%s",securities_code);
	if(day%2==0&&isdigit(securities_code[0])==0){
		system("cls");
		setColor(12,0);
		printf("현재는 오전이므로 한국주식만 거래가 가능합니다.\n\n");
		setColor(7,0);
		return; 
	}
	else if(day%2==1&&isdigit(securities_code[0])==1){
		system("cls");
		setColor(12,0);
		printf("현재는 오후이므로 미국주식만 거래가 가능합니다.\n\n");
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
		printf("고객님이 보유하고 있지 않은 주식입니다. 다시 확인해주세요.\n\n");
		setColor(7,0);
		return;
	}
	printf("%s %d주 중 몇주를 매도하겠습니까?--->",code_finder(atmp->securities_code)->name,atmp->amount);
	scanf("%d",&amount);
	system("cls");
	if(atmp->amount<amount){
		setColor(12,0);
		printf("보유한 주식보다 많은 주식을 매도할 수 없습니다. 다시 확인해주세요.\n\n");
		setColor(7,0);
		return;
	}
	printf("%s %d주를 매도하시겠습니까? 총 ",code_finder(atmp->securities_code)->name,amount);
	if(day%2==0)
		comma(amount*code_finder(atmp->securities_code)->price);
	else
		comma(amount*code_finder(atmp->securities_code)->price*exchange);
	printf("원이 입금됩니다.\n1: 예\n그 외키: 아니요\n");
	key=getch();
	if(key!='1'){
		setColor(12,0);
		printf("매도 주문이 취소되었습니다.\n\n");
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
	printf("%s %d주 매도가 완료되었습니다. 잔여 주는 %d주 이며, 총 예수금은 ",code_finder(atmp->securities_code)->name,amount,atmp->amount);
	comma(tmp->balance);
	printf("원입니다.\n\n");
	setColor(7,0);
	if(atmp->amount==0)
		delete_account(tmp,atmp);
}

void deposit(){//입금 
	char code[5];
	long long balance;
	client *tmp;
	int key;
	
	showall_client();
	printf("입금하는 고객의 코드를 입력하세요--->");
	scanf("%s", code);
	tmp=client_finder(code);
	if(tmp==NULL){
		system("cls");
		setColor(12,0);
		printf("고객코드를 잘못 입력했습니다. 다시 확인해주세요.\n\n");
		setColor(7,0);
		return;
	}
	system("cls");
	printf("%s(%s)고객님의 잔고는 ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("원입니다. 얼마를 입금하겠습니까? \n");
	scanf("%lld",&balance);
	system("cls");
	comma(balance);
	printf("원을 입금시 총 예수금은 ");
	comma(tmp->balance+balance);
	printf("원입니다. 입금을 진행하시겠습니까?\n\n1:예\n그 외키:아니오");
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("입금이 취소되었습니다.");
		setColor(7,0);
		return;
	} 
	tmp->balance=tmp->balance+balance;
	setColor(10,0);
	printf("입금이 완료되었습니다.%s(%s)고객님의 총 예수금은 ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("원입니다.\n\n");
	setColor(7,0);
}

void withdrawl(){//출금 
	int key;
	char code[5];
	client *tmp;
	long long balance;
	
	showall_client();
	printf("출금하는 고객의 코드를 입력하세요--->");
	scanf("%s",code);
	tmp=client_finder(code);
	if(tmp==NULL){
		system("cls");
		setColor(12,0);
		printf("존재하지 않는 고객코드 입니다. 다시 확인해주세요\n\n");
		setColor(7,0);
		return;
	}
	printf("%s(%s)고객님의 잔고는 ",tmp->name,tmp->code);
	comma(tmp->balance);
	printf("원입니다. 얼마를 출금하겠습니까?\n------>");
	scanf("%lld",&balance);
	system("cls");
	if(tmp->balance<balance){
		setColor(12,0);
		printf("예수금 보다 큰 금액은 출금할 수 없습니다. 다시 확인해주시기 바랍니다.\n\n");
		setColor(7,0);
		return; 
	}
	comma(balance);
	printf("원을 출금하겠습니까? 출금시 잔액은 ");
	comma(tmp->balance-balance);
	printf("원입니다.\n\n1: 예\n그 외키:아니오\n");
	key=getch();
	if(key!='1'){
		setColor(12,0);
		printf("출금이 취소되었습니다.\n\n");
		setColor(7,0);
		return;
	}
	tmp->balance=tmp->balance-balance;
	setColor(10,0);
	printf("출금이 완료되었습니다. %s(%s)고객님의 총 예수금은 ",tmp->name,tmp->code);
	comma(balance);
	printf("원입니다.\n\n");
	setColor(7,0);
}

void new_customer(){//신규고객 
	int key;
	char name[20],code[5];
	long long balance;
	client *tmp;
	
	printf("신규고객 등록을 진행하시겠습니까?\n1: 예\n그 외키: 아니요");
	key=getch();
	if(key!='1'){
		system("cls");
		setColor(12,0);
		printf("신규고객 등록이 취소되었습니다.\n\n");
		setColor(7,0);
		return;
	}
	system("cls");
	printf("신규고객의 성함을 입력해주세요.\n");
	scanf("%s",name);
	
	system("cls");
	existing_code();
	printf("원하는 고객코드를 입력하세요. 코드는 네자리 숫자로 이루어져야 하고 중복될 수 없습니다.\n");
	scanf("%s",&code);
	for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
		if(strcmp(tmp->code,code)==0)
			return;
	}
	while(tmp!=NULL){
		system("cls");
		existing_code();
		setColor(12,0);
		printf("\n\n코드가 중복되었습니다. 다시 입력해주세요.\n");
		setColor(7,0);
		scanf("%s",code);
		for(tmp=head_client;tmp!=NULL;tmp=tmp->next){
			if(strcmp(tmp->code,code)==0)
				return;
		}
	}
	printf("입금할 금액은 얼마입니까?\n");
	scanf("%lld",&balance);
	
	system("cls");
	printf("%s(%s) 고객님을 등록하시겠습니까?\n1: 예\n그 외키: 아니요\n",name,code);
	key=getch();
	if(key!='1'){
		setColor(12,0);
		printf("신규고객 등록이 취소되었습니다.\n\n");
		setColor(7,0);
		return;
	}
	system("cls");
	setColor(10,0);
	printf("%s고객님 등록이 완료되었습니다.\n\n",name);
	setColor(7,0);
	addFirst_client(code,name,balance);
}

void remove_customer(){//고객탈퇴 
	client *tmp, *tmp2;
	client_account *atmp;
	int key;
	char code[5];
	
	printf("고객탈퇴를 진행하겠습니까?\n1: 예\n그 외키: 아니요\n");
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("고객탈퇴가 취소되었습니다.\n\n");
		setColor(7,0);
		return;
	}
	showall_client();
	printf("탈퇴하는 고객의 코드를 입력하세요--->");
	scanf("%s",code);
	system("cls");
	tmp=client_finder(code);
	if(tmp==NULL){
		setColor(12,0);
		printf("잘못된 고객코드입니다. 다시 확인해주세요");
		setColor(7,0);
		return;
	}
	printf("%s(%s) 고객님을 탈퇴처리하겠습니까?\n1: 예\n그 외키: 아니요\n",tmp->name,tmp->code);
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("고객탈퇴가 취소되었습니다.\n\n");
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
	printf("고객탈퇴가 완료되었습니다.\n\n");
	setColor(7,0);
}

void change_name(){//고객명변경 
	int key;
	client *tmp;
	char code[5], name[20];
	printf("고객명 변경을 진행하겠습니까?\n1: 예\n그 외키: 아니요\n");
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("고객명 변경이 취소되었습니다.\n\n");
		setColor(7,0);
		return;
	}
	showall_client();
	printf("\n\n고객명 변경을 진행할 고객의 코드를 입력해주세요---->");
	scanf("%s",code);
	system("cls");
	tmp=client_finder(code);
	if(tmp==NULL){
		setColor(12,0);
		printf("잘못된 고객코드입니다. 다시 확인해주세요.\n\n");
		setColor(7,0);
		return;
	}
	printf("%s(%s)고객님의 성함을 무엇으로 변경하겠습니까?--->",tmp->name,tmp->code);
	scanf("%s",name);
	system("cls");
	printf("%s(%s)고객님의 성함을 %s로 변경하겠습니까?\n1: 예\n그 외키: 아니요\n",tmp->name,tmp->code,name);
	key=getch();
	system("cls");
	if(key!='1'){
		setColor(12,0);
		printf("고객명 변경이 취소되었습니다.\n\n");
		setColor(7,0);
		return;
	}
	setColor(10,0);
	printf("%s고객님 성함이 %s고객님으로 변경이 완료되었습니다.\n\n",tmp->name,name);
	setColor(7,0);
	strcpy(tmp->name,name);
}

void exchange_rate() {//환율변동
	srand(time(NULL));
	int rate, pn;
	rate = rand() % 40;
	pn = rand() % 2;
	if (pn == 0)
		exchange = exchange + exchange / 1000 * rate;
	else
		exchange = exchange - exchange / 1000 * rate;
	printf("\n\n현재 환율 1USD = %d,%dKRW", exchange/1000, exchange%1000);
}

void homescreen_am() {
	printf("지금은 %d일차 오전입니다. 한국주식을 거래할 수 있습니다.\n\n", day / 2 + 1);
	printf("종목코드  종목명    1주당 가격  전날대비변동률\n\n");
	showall();
	printf("\n--------------------------------------------------------\n");
	printf("1. 고객관리(종목매매, 입출금, 신규고객, 고객탈퇴)\n");
	printf("2. 종목관리(신규상장 및 폐지)\n");
	printf("3. 장마감\n");
	printf("0. 프로그램 종료\n");
	printf("--------------------------------------------------------\n");
	printf("번호를 입력하세요--->");
}

void homescreen_pm() {
	printf("지금은 %d일차 오후입니다. 미국주식을 거래할 수 있습니다.\n\n", day / 2+1);
	printf("종목코드  종목명    1주당 가격  전날대비변동률\n\n");
	showall();
	exchange_rate(); 
	printf("\n--------------------------------------------------------\n");
	printf("1. 고객관리(종목매매, 입출금, 신규고객, 고객탈퇴)\n");
	printf("2. 종목관리(신규상장 및 폐지)\n");
	printf("3. 장마감\n");
	printf("0. 프로그램 종료\n");
	printf("--------------------------------------------------------\n");
	printf("번호를 입력하세요--->");
}

void price_fluct() {//주식가격 변동 
	int rate, pn, percent;
	securities *tmp;
	srand(time(NULL));
	if (day%2==0) //한국주식 변동 
		tmp=head_kr;
	else //미국주식 변동 
		tmp=head_us;
		
	while(tmp!=NULL){
		percent=rand()%100;
		if(percent<90){//90%확률로 주가 변동이 0~16%일어나고 10%확률로 주가 변동이 17~30%까지 일어남 
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

void init() {//securities.txt로부터 linked list 생성
	FILE *fp;
	int price, pn, rate;
	char code[7], name[40];
	fp = fopen("securities.txt", "r");
	fscanf(fp,"%d",&day);
	fscanf(fp,"%d",&exchange);
	if (fp == NULL) {
		setColor(12,0);
		printf("종목등록이 되어 있지 않습니다.\n");
		setColor(7,0);
		return;
	}
	while (!feof(fp)) {
		fscanf(fp, "%s", code);
		fscanf(fp, "%s", name);
		fscanf(fp, "%d", &price);
		fscanf(fp, "%d", &pn);
		fscanf(fp, "%d", &rate);
		if (isdigit(code[0])!=0){//종목코드가 숫자로 시작하면 한국, 영어로 시작하면 미국
			addFirst(0, code, name, price,pn,rate);
		}
		else{
			addFirst(1, code, name, price,pn,rate);
		}
	}
	fclose(fp);
}

void init_client(){//고객정보 linked list생성
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

void init_account(){//고객이 보유한 주식 linked list 생성
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

void cmanage(){//고객관리 
	int key, amount, i, same;
	long long balance;
	char code[5], securities_code[7], name[20];
	client *tmp,*tmp2;
	client_account *atmp,*atmp2;
	securities *stmp;
	srand(time(NULL));
	
	while(1){
		printf("고객코드 고객명  예수금  총 잔고\n\n");
		showall_client();
		printf("-----------------------------\n");
		printf("1. 보유주식 조회\n");
		printf("2. 매수\n");
		printf("3. 매도\n");
		printf("4. 입금\n");
		printf("5. 출금\n");
		printf("6. 신규고객\n");
		printf("7. 고객탈퇴\n");
		printf("8. 고객명 변경\n\n");
		printf("Backspace: 메인화면으로\n"); 
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

void sell_all(securities *node){//대체 뭐가 문제일까
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

void smanage(){//주식관리 
	int key, price, mode,i;
	char code[7], name[40];
	securities *tmp, *smode, *tmp2;
	
	while(1){
		printf("종목코드 종목명   1주당 가격  전날대비변동률\n\n");
		showall();
		printf("------------------------------\n");
		printf("1. 신규상장\n");
		printf("2. 상장폐지\n\n");
		printf("Backspace: 메인화면으로");
		
		key=getch();
		switch(key){
			case '1'://신규상장
				system("cls");
				printf("신규상장을 진행하시겠습니까?\n1: 예\n그 외키: 아니오\n");
				key=getch();
				if(key=='1'){;
					system("cls");	
					printf("신규상장할 종목코드를 입력하세요.\n");
					scanf("%s", code);//같은 종목코드가 있는지 확인 
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
						printf("이미 존재하는 종목입니다. 다시 확인해주세요\n\n");
						setColor(7,0);
						break;
					}
				
					printf("\n신규상장할 종목이름을 입력하세요.\n");
					scanf("%s", name);
				
					printf("\n신규상장할 종목의 1주당 가격을 입력하세요.\n");
					scanf("%d",&price);
					
					system("cls");
					printf("%s %s ", code, name);
					comma(price);
					if(day%2==0)
						printf("원\n");
					else
						printf("달러\n");
						
					printf("\n정말 이 정보로 신규상장을 하시겠습니까?\n1: 예\n그 외키: 아니오");
					
					key=getch(); 
					
					if(key=='1'){
						system("cls");
						addFirst(day%2, code, name, price,0,0);
						setColor(10,0);
						printf("신규상장이 완료되었습니다.\n\n");
						setColor(7,0);
					}
					else{
						system("cls");
						setColor(12,0);
						printf("신규상장이 취소되었습니다.\n\n");
						setColor(7,0);
					}
					free(tmp);
				}
				else
					system("cls");
					
				break;
				
			case'2'://상장폐지, 종목폐지할 때 고객이 해당 주를 가지고 있으면 전량 매도 처리
				system("cls");
				printf("종목폐지를 진행하시겠습니까?\n1: 예\n그 외키: 아니오");
				key=getch();
				
				if(key!='1'){
					system("cls");
					setColor(12,0);
					printf("종목폐지가 취소되었습니다.\n\n");
					setColor(7,0);
					break;
				}
				system("cls");
				showall();
				printf("\n--------------------------------------\n");
				printf("상장폐지할 종목코드를 입력하세요.\n");
				scanf("%s", code);
				system("cls");
				//일치하는 종목 찾고 없으면 에러 출력
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
					printf("존재하지 않는 코드입니다. 다시 확인해주세요.\n\n");
					setColor(7,0);
					break;
				}
				system("cls");
				printf("%s %s\n",tmp->code, tmp->name);
				printf("정말로 이 종목을 상장폐지하시겠습니까? 이 종목의 주식을 보유한 고객의 모든 잔량이 매도됩니다.\n\n1: 예\n그 외키: 아니오");
				
				key=getch();
					
				if(key!='1'){
					system("cls");
					setColor(12,0);
					printf("종목폐지가 취소되었습니다.\n\n");
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
				printf("종목폐지가 완료되었습니다.\n\n");
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

//텍스트 파일 출력 
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
	init(); //종목 리스트 읽은 후 한국과 미국 구분하여 생성, 코드가 숫자로 시작하면 한국, 영어로 시작하면 미국
	init_client();//고객정보 리스트 생성
	init_account();//보유주식수 리스트 생성
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
				case '1'://고객관리
					system("cls");
					cmanage();
					break;
				case'2'://종목관리
					system("cls");
					smanage();
					break;
				case'3':
					system("cls");
					printf("현재 장을 마감하겠습니까?\n3: 예\n그 외키: 아니요\n");
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
					printf("정말로 프로그램을 종료하겠습니까?");
					setColor(7,0);
					printf("\n\n1: 예\n그 외키: 아니오");
					key=getch();
					if(key=='1'){
						system("cls");
						printf("Bye Bye");
						terminate();//모든 상황 텍스트파일에 저장
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
