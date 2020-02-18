//
// Created by Killer Will on 2019/12/9.
//

#ifndef P3_LATCH_REG_H
#define P3_LATCH_REG_H
#include <string>
using namespace std;
static class latch_reg {
private:
    unsigned long int IR;
    unsigned long int NPC;
    unsigned long int A;
    unsigned long int B;
    unsigned int WR;
    int Imm;
    unsigned long int UImm;
    string opcode;
    unsigned long int ALUOutput;
    unsigned long int ALUOutput2;
    bool cond;
    unsigned long int LMD;

public:
    static unsigned long int PC;
    static unsigned long int R[32];
    static bool NOP;
    double ut;
    double ut2;

    void setIR(unsigned long int n){
        IR=n;
    }
    unsigned long int getIR(){
        return IR;
    }
    void setWR(unsigned int n){
        WR=n;
    }
    unsigned int getWR(){
        return WR;
    }

    void setNPC(unsigned long int n){
        NPC=n;
    }

    unsigned long int getNPC(){
        return NPC;
    }
    void setA(unsigned long int n){
        A=n;
    }
    unsigned long int getA(){
        return A;
    }
    void setB(unsigned long int n){
        B=n;
    }
    unsigned long int getB(){
        return B;
    }
    void setImm(int n){
        Imm=n;
    }
    int getImm(){
        return Imm;
    }
    void setUImm(unsigned long int n){
        UImm=n;
    }
    unsigned long int getUImm(){
        return UImm;
    }

    void setopcode(string n){
        opcode=n;
    }
    string getopcode(){
        return opcode;
    }
    void setALUOutput(unsigned long int n){
        ALUOutput=n;
    }
    unsigned long int getALUOutput(){
        return ALUOutput;
    }
    void setALUOutput2(unsigned long int n){
        ALUOutput2=n;
    }
    unsigned long int getALUOutput2(){
        return ALUOutput2;
    }
    void setcond(bool n){
        cond=n;
    }
    bool getcond(){
        return cond;
    }
    void setLMD(unsigned long int n){
        LMD=n;
    }
    unsigned long int getLMD(){
        return LMD;
    }
};


#endif //P3_LATCH_REG_H
