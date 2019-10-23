//
// Created by andrew on 11.10.2019.
//


DEF_CMD(PUSH, 1,
        { Stack_Push(stk1, *(int*)(buf+PC)); PC += sizeof(int); },
                              {
                                    sscanf(current_line + position, "%s", arg);
                                    if (arg[0] != '\0') {
                                        if (sscanf(arg, "%d", &num)) {
                                            *(int*) (assem + PC) = num * ACCURACY ;
                                            PC +=4;
                                        }
                                        else CHOSE_REG(CMD_PUSH_REG)
                                        else
                                        ERR
                                    }
                                    else ERR
                             },
                             { arg = *(int*)(buf + PC) / ACCURACY; PC += 4;   fprintf(flout, " %d\n", arg); })



DEF_CMD (ADD, 2, {OPERATION(+)}, {} ,{fputc('\n', flout);})

DEF_CMD (SUB, 3, {OPERATION(-) }, {}, {fputc('\n', flout);})

DEF_CMD (POP, 4, {X;}, { sscanf(current_line + position, "%s", arg);
                            if (arg[0] != '\0') {
                                 CHOSE_REG(CMD_POP_REG)
                            else ERR
                            }}, {fputc('\n', flout);})


DEF_CMD (MUL, 5, {OPERATION(*) }, {} ,{fputc('\n', flout);})

DEF_CMD (DIV, 6, {OPERATION(/)}, {} ,{fputc('\n', flout);})

DEF_CMD (POW, 7, { float y = Y; $(Power(y, X)) }, {} , {fputc('\n', flout);})

DEF_CMD (OUT, 8, { float y = Y; PRINT_digit(y); $(y * ACCURACY); }, {}, {fputc('\n', flout);})

DEF_CMD (CALL, 16,
        {
            Stack_Push(stk_calls, PC + 4);
            PC = *(int*)(buf + PC) - SIGNATURE_SIZE - VERSION_SIZE;
        },
        { LABEL },
        { FPUTARG })

DEF_CMD (RET, 17, { PC = Stack_Pop(stk_calls); }, {} ,{fputc('\n', flout);})

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


DEF_CMD (END, 0,
        { //Stack_Dump(stk1);
            PC = length; }, {}, {fputc('\n', flout);})


DEF_CMD (PUSH_REG, 100, {Stack_Push(stk1, Registers[*(buf + PC++)]);}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })
DEF_CMD (POP_REG, 101,  {Registers[*(buf + PC++)] = Stack_Pop(stk1);}, {} , {arg = (int)*(buf + PC++);   fprintf(flout, " %d\n", arg); })

