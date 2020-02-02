//
// Created by andrew on 11.10.2019.
//


DEF_CMD(PUSH, 1,
        { Stack_Push(stk1, *(int*)(buf+PC)); PC += sizeof(int); },
                              {
                                    int temp = 0;
                                    sscanf(current_line + position, "%*[ \t]%n", &temp);
                                    position += temp;
                                    if (*(current_line + position) == '[') {
                                            assem[PC-1] = CMD_PUSH_OPER_NUM;
                                            sscanf(current_line + position, "[%[^]]]", arg);
                                            //printf("%s\n", arg);
                                        if (isalpha(*arg)){
                                            CHOSE_REG(CMD_PUSH_OPER_REG)
                                        }
                                        else if(temp = atoi(arg) || *arg == '0'){
                                            *(int*)(assem+PC) = temp;
                                           // printf("%s\n", arg);
                                            PC += 4;
                                        }
                                         else ERR

                                    }
                                    else if (isalpha(*(current_line + position))){
                                        sscanf(current_line + position, " %s", arg);
                                        CHOSE_REG(CMD_PUSH_REG)
                                    }
                                    else if((temp  = (int)(strtod(current_line + position, nullptr) * ACCURACY)) || *(current_line + position) == '0'){
                                        //printf("%d \n", temp);
                                        //printf("%d %s in line %d \n ", position, current_line + position, i+1);
                                        *(int*)(assem+PC) = temp;
                                        PC += 4;
                                    } else ERR
                             },
                             { arg = *(int*)(buf + PC) / ACCURACY; PC += 4;   fprintf(flout, " %d\n", arg); })



DEF_CMD (ADD, 2, {OPERATION(+)}, {} ,{fputc('\n', flout);})

DEF_CMD (SUB, 3, {OPERATION(-) }, {}, {fputc('\n', flout);})

DEF_CMD (POP, 4, {X;}, {
                            int temp = 0;
                            sscanf(current_line + position, "%*[ \t]%n", &temp);
                            position += temp;

                            if (*(current_line + position) == '[') {
                                assem[PC-1] = CMD_POP_OPER_NUM;
                                sscanf(current_line + position, " [%[^]]]", arg );

                                if (isalpha(*(arg))){
                                    CHOSE_REG(CMD_POP_OPER_REG)
                                }
                                else if(temp = atoi(arg) || *(arg) == '0'){
                                    *(int*)(assem+PC) = temp;
                                    PC += 4;
                                } else ERR

                            }
                            else if (isalpha(*(current_line + position))){
                                sscanf(current_line + position, " %s", arg);
                                CHOSE_REG(CMD_POP_REG)
                            }
                            else ERR
                            }, {fputc('\n', flout);})


DEF_CMD (MUL, 5, {OPERATION(*) }, {} ,{fputc('\n', flout);})

DEF_CMD (DIV, 6, {OPERATION(/)}, {} ,{fputc('\n', flout);})

DEF_CMD (POW, 7, { float y = Y; $((int)(pow(X, y) * ACCURACY)) }, {} , {fputc('\n', flout);})

DEF_CMD (OUT, 8, { float y = Y; PRINT_digit(y);  }, {
            int temp = 0;
            sscanf(current_line + position, "%*[ \t]%n", &temp);
            position += temp;

            if (*(current_line + position) == '[') {
            assem[PC-1] = CMD_OUT_OPER_NUM;
            sscanf(current_line + position, " [%[^]]]", arg );

            if (isalpha(*(arg))){
            CHOSE_REG(CMD_OUT_OPER_REG)
            }
            else if(temp = atoi(arg) || *(arg) == '0'){
            *(int*)(assem+PC) = temp;
            PC += 4;
            } else ERR

            }
            else if (isalpha(*(current_line + position))){
            sscanf(current_line + position, " %s", arg);
            CHOSE_REG(CMD_OUT_REG)
            }
            //else ERR
    }, {fputc('\n', flout);})

DEF_CMD (JMP, 9,
{ PC = *(int*)(buf + PC) - SIGNATURE_SIZE - VERSION_SIZE;},
{ LABEL },
{ FPUTARG })

DEF_CMD (JA, 10,
{ JUMPTO_IF(>) },
{ LABEL },
{ FPUTARG })

DEF_CMD (JB, 11,
{ JUMPTO_IF(<) },
{ LABEL },
{ FPUTARG })

DEF_CMD (JAE, 12,
{ JUMPTO_IF(>=) },
{ LABEL },
{ FPUTARG })

DEF_CMD (JBE, 13,
{ JUMPTO_IF(<=) },
{ LABEL },
{ FPUTARG })

DEF_CMD (JE, 14,
{ JUMPTO_IF(==) },
{ LABEL },
{ FPUTARG })

DEF_CMD (JNE, 15,
{ JUMPTO_IF(!=) },
{ LABEL },
{ FPUTARG })

DEF_CMD (CALL, 16,
        {
            Stack_Push(stk_calls, PC + 4);
            PC = *(int*)(buf + PC) - SIGNATURE_SIZE - VERSION_SIZE;
        },
        { LABEL },
        { FPUTARG })

DEF_CMD (RET, 17, { PC = Stack_Pop(stk_calls); }, {} ,{fputc('\n', flout);})

DEF_CMD (SQRT, 18, {  $(sqrt(Y) * ACCURACY) }, {} , {fputc('\n', flout);})
DEF_CMD (SIN, 19, {  $(sin(Y) * ACCURACY) }, {} , {fputc('\n', flout);})
DEF_CMD (COS, 20, {  $(cos(Y) * ACCURACY) }, {} , {fputc('\n', flout);})

DEF_CMD (IN, 21, {X;}, {

                    int temp = 0;
                    sscanf(current_line + position, "%*[ \t]%n", &temp);
                    position += temp;

                    if (*(current_line + position) == '[') {
                    assem[PC-1] = CMD_IN_OPER_NUM;
                    sscanf(current_line + position, " [%[^]]]", arg );

                    if (isalpha(*(arg))){
                        CHOSE_REG(CMD_IN_OPER_REG)
                    }
                    else if(temp = atoi(arg) || *(arg) == '0'){
                        *(int*)(assem+PC) = temp;
                        PC += 4;
                    } else ERR

                    }
                    else if (isalpha(*(current_line + position))){
                        sscanf(current_line + position, " %s", arg);
                        CHOSE_REG(CMD_IN_REG)
                    }
                    else ERR


    }, {})

DEF_CMD (IN_REG, 22, {
    printf("IN_REG %d:", *(buf+PC)) ;
float temp = 0;
scanf("%f" , &temp);
Registers[*(buf + PC++)] = (int)(temp * ACCURACY);
                            }, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })



DEF_CMD (IN_OPER_NUM, 23, {
printf("IN_OPER %d:", *(int*)(buf + PC)) ;
float temp = 0;
scanf("%f" , &temp);
Operative[*(int*)(buf + PC)] = (int)(temp * ACCURACY);
PC += 4;}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })



DEF_CMD (IN_OPER_REG, 24, {
printf("IN_OPER %d:", (int)(Registers[*(buf + PC)]/ACCURACY)) ;
float temp = 0;
scanf("%f" , &temp);
Operative[(int)(Registers[*(buf + PC++)]/ACCURACY)] = (int)(temp * ACCURACY);
}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })


DEF_CMD (LOG_E, 25, {  $(log(Y) * ACCURACY) }, {} , {fputc('\n', flout);})

DEF_CMD (OUT_REG, 26, {
printf("OUT_REG %d: ", *(buf+PC)) ;
printf("%.3f" , Registers[*(buf + PC++)]/ACCURACY);
}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })



DEF_CMD (OUT_OPER_NUM, 27, {
printf("OUT_OPER %d: ", *(int*)(buf + PC)) ;
printf("%.3f" , Operative[*(int*)(buf + PC)] / ACCURACY);
PC += 4;}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })



DEF_CMD (OUT_OPER_REG, 28, {
printf("OUT_OPER %d: ", (int)(Registers[*(buf + PC)]/ACCURACY)) ;
printf("%.3f" , Operative[(int)(Registers[*(buf + PC++)]/ACCURACY)] / ACCURACY);
}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })



DEF_CMD (END, 0,
        { //Stack_Dump(stk1);
            PC = length; }, {}, {fputc('\n', flout);})


DEF_CMD (PUSH_REG, 100, {Stack_Push(stk1, Registers[*(buf + PC++)]);}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })
DEF_CMD (POP_REG, 101,  {Registers[*(buf + PC++)] = Stack_Pop(stk1);}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })

DEF_CMD (PUSH_OPER_REG, 102, {Stack_Push(stk1, Operative[(int)(Registers[*(buf + PC++)]/ACCURACY)]);}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })
DEF_CMD (POP_OPER_REG, 103,  {Operative[(int)(Registers[*(buf + PC++)]/ACCURACY)] = Stack_Pop(stk1);}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })

DEF_CMD (PUSH_OPER_NUM, 104, { Stack_Push(stk1, Operative[*(int*)(buf+PC)]); PC += sizeof(int); }, {} , {arg = *(int*)(buf + PC) / ACCURACY; PC += 4;   fprintf(flout, " %d\n", arg); })
DEF_CMD (POP_OPER_NUM, 105,  {Operative[*(int*)(buf + PC)] = Stack_Pop(stk1); PC += 4;}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })
