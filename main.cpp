#include <iostream>
using namespace std;
#include <sstream>
#include <string>
#include "latch_reg.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
latch_reg IF_ID;
latch_reg ID_EX;
latch_reg EX_MEM;
latch_reg MEM_WB;
unsigned long int latch_reg::PC=0;
unsigned long int latch_reg::R[32]={0};
bool NOP=false;
vector<unsigned int> IMem={};
unsigned long int DMem[2048]={0};
void IF_STAGE();
int ID_STAGE();
void EX_STAGE();
void MEM_STAGE();
void WB_STAGE();

int main() {
/**File Input**/
    fstream data;
    string line;
    string filename="/Users/killerwill/CLionProjects/P3/MIPSInstruction.txt";
    data.open(filename.c_str());
    IMem.clear();
    while(getline(data,line)) {
        for (int i = 0; i < line.size(); i += 8) {
            int m=0;
            for (int j = i+7; j > i-1 ; j--) {
                stringstream ss;
                int temp;
                ss << line[j];
                ss >> temp;
                m += temp * pow(2, i+7-j);
            }
            IMem.push_back(m);
        }
    }
    /**
    int hazard = 0;
    for (int i = 0; i < (IMem.size()/4 + 4); i++) {
        WB_STAGE();
        MEM_STAGE();
        EX_STAGE();
        hazard = ID_STAGE();
        IF_STAGE();
        if (hazard == -1)
            i--;
    }

 /**Initialization**/
 IF_ID.ut=0;
 ID_EX.ut=0;
 EX_MEM.ut=0;
 MEM_WB.ut=0;
 MEM_WB.ut2=0;
time_t start,finish;
double total_time;
string Mode;
int INST_NUM;
cout<<"SELECT MODE: Cycle Mode or Instruction Mode? "<<endl;
cin>>Mode;

string initial="yes";
/**TESTBENCH**/
while(initial=="yes") {
JUGDE:
    cout<<"How many instructions or cycles to take? "<<endl;
    cin>>INST_NUM;
    start=clock();
    int hazard = 0;
    int clk;
     if(Mode=="instruction"){
        clk=0;
        for (int i = 0; i < (INST_NUM+4); i++) {
        WB_STAGE();
        MEM_STAGE();
        EX_STAGE();
        hazard = ID_STAGE();
        IF_STAGE();
        if (hazard == -1)//STALL
            i--;
        cout<<"CC: "<<clk<<endl;
        clk++;
      }
    }
    else if(Mode=="cycle"){
        clk=0;
        int hazard = 0;
        for (int i = 0; i < (INST_NUM); i++) {
        WB_STAGE();
        MEM_STAGE();
        EX_STAGE();
        hazard = ID_STAGE();//No Stalling in cycle mode
        IF_STAGE();
         cout<<"CC: "<<clk<<endl;
         clk++;
    }}finish=clock();
    cout<<"Do you want to try again? "<<endl;
    cin>>initial;
    if(initial=="yes")
       {//Reinitialization
         latch_reg::PC=0;
         IF_ID.setIR(0);
         ID_EX.setIR(0);
         EX_MEM.setIR(0);
         MEM_WB.setIR(0);
         IF_ID.ut=0;
         ID_EX.ut=0;
         EX_MEM.ut=0;
         MEM_WB.ut=0;
         MEM_WB.ut2=0;

         for(int i=0;i<9;i++)
            latch_reg::R[i+8]=0;
        goto JUGDE;
       }
    else{
        total_time=(double)(finish-start)/CLOCKS_PER_SEC*1000;
        cout<<"Register Output: "<<endl;
        for(int i=0;i<8;i++){
            cout<<"$t"<<i<<" = 0x";
            printf("%x",latch_reg::R[i+8]);
            cout<<endl;
        }
        cout<<endl;
        cout<<"Data Memory Output: "<<endl;
        for(int i=0;i<14;i+=4){
            cout<<"Data in DMem["<<i<<"-"<<i+3<<"] is 0x";
            printf("%x",DMem[i]);
            printf("%x",DMem[i+1]);
            printf("%x",DMem[i+2]);
            printf("%x",DMem[i+3]);
            cout<<endl;
        }
        cout<<endl;
        cout<<"The execution time of this MIPS simulator is "<<total_time<<"ms"<<endl;
        cout<<endl;
        cout<<"Utilization of IF STAGE is "<<(double)(IF_ID.ut/clk)/5*100<<"%"<<endl;
        cout<<"Utilization of ID STAGE is "<<(double)(ID_EX.ut/clk)/5*100<<"%"<<endl;
        cout<<"Utilization of EX STAGE is "<<(double)(EX_MEM.ut/clk)/5*100<<"%"<<endl;
        cout<<"Utilization of MEM STAGE is "<<(double)(MEM_WB.ut/clk)/5*100<<"%"<<endl;
        cout<<"Utilization of WB STAGE is "<<(double)(MEM_WB.ut2/clk)/5*100<<"%"<<endl;
    }
}
/**TEST**/
    //printf("%x",latch_reg::R[9]);
  // printf("%x",DMem[8]);
}
void IF_STAGE(){
    unsigned int address=latch_reg::PC;
    if(NOP){
        IF_ID.setIR(0);
        return; }/**WHEN NOP, stop fetching**/
    if(address>=IMem.size()){/**When the Instructions all fetched, Stop fetch**/
        IF_ID.setIR(0);
        return;
    }
    /**Utilization**/
    if((IF_ID.getIR()!=0)&&(latch_reg::PC!=IF_ID.getNPC()))
        IF_ID.ut++;
    /*****************/

    unsigned int instr=(IMem[address]<<24)|
                       (0x00FF0000&(IMem[address+1]<<16))|
                       (0x0000FF00&(IMem[address+2]<<8))|
                       (0x000000FF&IMem[address+3]);
    if(instr==0)
        return;
    IF_ID.setIR(instr);


    if((EX_MEM.getopcode()=="BRANCH")&&(EX_MEM.getcond())){
        IF_ID.PC=EX_MEM.getALUOutput();
        IF_ID.setNPC(EX_MEM.getALUOutput());
    }
    else{
        IF_ID.PC+=4;
        IF_ID.setNPC(IF_ID.PC);
    }

}
int ID_STAGE(){
    unsigned int rs=(IF_ID.getIR()&0x03E00000)>>21;
    unsigned int rt=(IF_ID.getIR()&0x001F0000)>>16;
    unsigned int rd=(IF_ID.getIR()&0x0000F800)>>11;
    unsigned int funct=IF_ID.getIR()&0x0000003F;//Funct field
    short int op=IF_ID.getIR()>>26;//Oprand field
    ID_EX.setNPC(IF_ID.getNPC());
    ID_EX.setIR(IF_ID.getIR());
    if(IF_ID.getIR()==0)//No Instructions in IF_ID, do not DECODE
        return 0;
    /**Sign extend**/
    unsigned long int m=0X0000FFFF&ID_EX.getIR();
    ID_EX.setUImm(m);//Unsigned Immediate here
    int i;
    if(m<=0x8000)
        ID_EX.setImm(i=0X0000FFFF&m);
    else
        ID_EX.setImm(i=0XFFFF0000|m);
    /**DATA HAZARD and STRUCTURE HAZARD**/
    if(((EX_MEM.getWR()==rs)||(EX_MEM.getWR()==rt)||(MEM_WB.getWR()==rt)||
    (MEM_WB.getWR()==rs))&&((EX_MEM.getWR()!=0)||(MEM_WB.getWR()!=0))){
        latch_reg::PC-=4;
        ID_EX.setIR(0);
        return -1;}
    /**Utilization**/
     if(ID_EX.getIR()!=0)
         ID_EX.ut++;
    /**Decode the operation**/
    if(op==35)//should determine whether lw or sw,op=35 is lw
        ID_EX.setopcode("LW");//opcode==0b0010,ALU action is add;
    else if(op==43)//op is sw
        ID_EX.setopcode("SW");
    else if(op==4){
        ID_EX.setopcode("BRANCH");
        NOP=true;
        /**BRANCH HAZARD DETECTING**/
    }

    else if((op==0)&&(funct==32))
        ID_EX.setopcode("ADD");
    else if((op==0)&&(funct==34))
        ID_EX.setopcode("SUB");
    else if((op==0)&&(funct==36))
        ID_EX.setopcode("AND");
    else if((op==0)&&(funct==37))
        ID_EX.setopcode("OR");
    else if((op==0)&&(funct==24))
        ID_EX.setopcode("MUL");
    else if(op==8)
        ID_EX.setopcode("ADDI");
    else if(op==12)
        ID_EX.setopcode("ANDI");
    else if(op==13)
        ID_EX.setopcode("ORI");
    else if((op==15)&&(rs==0))
        ID_EX.setopcode("LUI");
    /**Read data from Register**/
    ID_EX.setA(latch_reg::R[rs]);
    ID_EX.setB(latch_reg::R[rt]);
    /**WriteReg or not **/
    if(op==0)//R-TYPE
        ID_EX.setWR(rd);
    else if((op==4)||(op==43))//BRANCH and SW
    {   ID_EX.setWR(23);
        return 0;}
    else
        ID_EX.setWR(rt);/**I-TYPE AND OTHERS expect beq**/
    return 0;

}
void EX_STAGE(){
    unsigned long result;
    if(ID_EX.getIR()==0){/**AVOID DATA HAZARD**/
        EX_MEM.setWR(0);
        return;
    }
    /**Utilization**/
    EX_MEM.ut++;
    /**START THE EXECUTION **/
    if((ID_EX.getopcode()=="LW")||(ID_EX.getopcode()=="SW")||(ID_EX.getopcode()=="ADDI"))
        result = ID_EX.getA() + ID_EX.getImm();
    else if(ID_EX.getopcode()=="ADD")
        result = ID_EX.getA()+ID_EX.getB();
    else if(ID_EX.getopcode()=="SUB")
        result = ID_EX.getA() - ID_EX.getB();
    else if(ID_EX.getopcode()=="BRANCH"){
        result = (ID_EX.getImm()<<2)+ID_EX.getNPC();
        EX_MEM.setcond((ID_EX.getA() - ID_EX.getB()==0)?true:false);
        EX_MEM.setALUOutput(result);
        NOP=false;//BRANCH is resolved
        EX_MEM.setIR(0);
        EX_MEM.setopcode("BRANCH");
        return;
    }
    else if(ID_EX.getopcode()=="ORI")
        result=(0x0000FFFF&ID_EX.getUImm())|ID_EX.getA();
    else if(ID_EX.getopcode()=="LUI")
        result=0xFFFF0000&(ID_EX.getUImm()<<16);
    else if(ID_EX.getopcode()=="ANDI")
        result=ID_EX.getA()&ID_EX.getUImm();
    else if(ID_EX.getopcode()=="AND")
        result=ID_EX.getA()&ID_EX.getB();
    else if(ID_EX.getopcode()=="OR")
        result=ID_EX.getA()|ID_EX.getB();
    else if(ID_EX.getopcode()=="MUL")
    {
        result=(ID_EX.getA())*(ID_EX.getB()>>16);//higher 32 bit
        EX_MEM.setALUOutput2(result);
        result=(ID_EX.getA())*(ID_EX.getB()&0x0000ffff);//lower 32 bit
        EX_MEM.setALUOutput(result);
        EX_MEM.setopcode(ID_EX.getopcode());
        EX_MEM.setWR(ID_EX.getWR());
        return;
    }

    else
        return;
    EX_MEM.setALUOutput(result);
    EX_MEM.setB(ID_EX.getB());
    EX_MEM.setopcode(ID_EX.getopcode());
    EX_MEM.setIR(ID_EX.getIR());
    EX_MEM.setWR(ID_EX.getWR());
}
void MEM_STAGE(){
    if(EX_MEM.getIR()==0)
    {
        MEM_WB.setIR(0);
        return;
    }
    /**Utilization**/
    if(EX_MEM.getopcode()!="BRANCH")
         MEM_WB.ut++;

    if(EX_MEM.getopcode()=="LW"){
        unsigned int address=EX_MEM.getALUOutput();
        int data=(DMem[address]<<24)|
                  (0x00FF0000&(DMem[address+1]<<16))|
                  (0x0000FF00&(DMem[address+2]<<8))|
                  (0x000000FF&DMem[address+3]);
        MEM_WB.setLMD(data);/**LMD:Load Memory Data **/
    }
    else if(EX_MEM.getopcode()=="SW"){
        unsigned int address=EX_MEM.getALUOutput();
        DMem[address]=((EX_MEM.getB()&0xFF000000)>>24);
        DMem[address+1]=((EX_MEM.getB()&0x00FF0000)>>16);
        DMem[address+2]=((EX_MEM.getB()&0x0000FF00)>>8);
        DMem[address+3]=((EX_MEM.getB()&0x000000FF));
        MEM_WB.setopcode(EX_MEM.getopcode());
        return;//No WB stage
    }
    else if(EX_MEM.getopcode()=="BRANCH"){
        MEM_WB.setopcode(EX_MEM.getopcode());
        return;
    }

    else if(EX_MEM.getopcode()=="MUL"){
        MEM_WB.setALUOutput(EX_MEM.getALUOutput());
        MEM_WB.setALUOutput2(EX_MEM.getALUOutput2());
    }
    else
        MEM_WB.setALUOutput(EX_MEM.getALUOutput());
    MEM_WB.setIR(EX_MEM.getIR());
    MEM_WB.setWR(EX_MEM.getWR());
    MEM_WB.setopcode(EX_MEM.getopcode());
}
void WB_STAGE() {
    if(MEM_WB.getIR()==0)
        return;
    /**Utilization**/
    if((MEM_WB.getopcode()!="SW")||(MEM_WB.getopcode()!="BRANCH"))
        MEM_WB.ut2++;

    if (MEM_WB.getopcode()=="LW")//LW
    {
        latch_reg::R[MEM_WB.getWR()] = MEM_WB.getLMD();
        MEM_WB.setWR(0);
    }
    else if(MEM_WB.getopcode()=="MUL")
    {
        latch_reg::R[MEM_WB.getWR()]=(MEM_WB.getALUOutput());//lower 32bit
        latch_reg::R[MEM_WB.getWR()+1]=(MEM_WB.getALUOutput2());//higher 32bit
    }
    else if((MEM_WB.getopcode()=="SW")||(MEM_WB.getopcode()=="BRANCH"))
        return;
    else /**R-TYPE & most I-TYPE**/
    {
        latch_reg::R[MEM_WB.getWR()] = MEM_WB.getALUOutput();
        MEM_WB.setWR(0);
    }
}