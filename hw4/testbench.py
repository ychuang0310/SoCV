from random import randint, shuffle, sample, seed
from tqdm import tqdm
from subprocess import Popen, PIPE, run

INPUT = "input/tmp"
OUTPUT = "tmp"
[CORRECT, ERR_ASSIGN, ERR_ASSIGNNOT, ERR_LESSTHAN, ERR_ADJACENT, ERR_ADJACENTNOT, ERR_FALSE_NEG, ERR_DOUBLE_ASS, ERR_LEN_MISMATCH] = [i for i in range(9)]
true_pos = 0
MAX_NUM = 50

def check(spec, out, golden_ass):
    ass_str = list()
    with open(out, "r") as f:
        line = f.readline()[:-1]
        if (line[0] == "S"):
            ass_str = f.readline().replace(" ", "").split(",")
        else:
            if golden_ass == []:
                return "CORRECT"
            else:
                return "ERR_FALSE_NEG"
    p_ass = [-1 for _ in range(len(ass_str))]
    s_ass = [-1 for _ in range(len(ass_str))]
    for ass in ass_str:
        [seat, person] = ass[:-1].split("(")
        [seat, person] = [int(seat), int(person)]
        if p_ass[person] != -1 or s_ass[seat] != -1:
            return ERR_DOUBLE_ASS
        p_ass[person] = seat
        s_ass[seat] = person
    
    with open(spec, "r") as f:
        num = int(f.readline()[:-1])
        if num != len(p_ass):
            return ERR_LEN_MISMATCH
        
        lines = f.readlines()
        for line in lines:
            [op, k_l_str] = line.replace("\n", "").split("(")
            [k, l] = k_l_str.split(" ")
            [k, l] = [int(k[:-1]), int(l[:-1])]
            if op == "Assign" and p_ass[k] != l:
                return "ERR_ASSIGN"
            elif op == "AssignNot" and p_ass[k] == l:
                return "ERR_ASSIGNNOT"
            elif op == "LessThan" and p_ass[k] > p_ass[l]:
                return "ERR_LESSTHAN"
            elif op == "Adjacent" and not (p_ass[k] == p_ass[l] + 1 or p_ass[k] + 1 == p_ass[l]):
                return "ERR_ADJACENT"
            elif op == "AdjacentNot" and (p_ass[k] == p_ass[l] + 1 or p_ass[k] + 1 == p_ass[l]):
                return "ERR_ADJACENTNOT"

    global true_pos
    true_pos += 1
    return "CORRECT"

def gen_ass():
    if randint(1, 3) != 1:
        return []
    num = randint(2, MAX_NUM)
    ass = [i for i in range(num)]
    shuffle(ass)
    return ass

def gen_spec(golden_ass):
    with open(INPUT, "w") as f:
        if golden_ass != []:
            num = len(golden_ass)
            gold_seat_ass = [0 for _ in range(num)]
            for i, seat in enumerate(golden_ass):
                gold_seat_ass[seat] = i
            f.write(str(num))
            num_op = randint(num, num*2)
            for _ in range(num_op):
                if num == 2:
                    op = randint(0, 3)
                else:
                    op = randint(0, 4)
                if op == 0:
                    k = randint(0, num-1)
                    l = golden_ass[k]
                    f.write("\nAssign({}, {})".format(k, l))
                elif op == 1:
                    [k, l] = sample(golden_ass, 2)
                    l = golden_ass[l]
                    f.write("\nAssignNot({}, {})".format(k, l))
                elif op == 2:
                    [k, l] = sample(golden_ass, 2)
                    if golden_ass[k] > golden_ass[l]:
                        k, l = l, k
                    f.write("\nLessThan({}, {})".format(k, l))
                elif op == 3:
                    k = randint(0, num-2)
                    l = gold_seat_ass[k+1]
                    k = gold_seat_ass[k]
                    f.write("\nAdjacent({}, {})".format(k, l))
                elif op == 4:
                    [k, l] = sample(golden_ass, 2)
                    while(k==l+1 or l==k+1):
                        [k, l] = sample(golden_ass, 2)
                    k = gold_seat_ass[k]
                    l = gold_seat_ass[l]
                    f.write("\nAdjacentNot({}, {})".format(k, l))
        else:
            num = randint(2, MAX_NUM)
            f.write(str(num))
            idxs = [i for i in range(num)]
            num_op = randint(num, num*2)
            for _ in range(num_op):
                f.write("\n")
                op = randint(0, 4)
                if op == 0:
                    [k, l] = sample(idxs, 2)
                    f.write("Assign({}, {})".format(k, l))
                elif op == 1:
                    [k, l] = sample(idxs, 2)
                    f.write("AssignNot({}, {})".format(k, l))
                elif op == 2:
                    [k, l] = sample(idxs, 2)
                    f.write("LessThan({}, {})".format(k, l))
                elif op == 3:
                    [k, l] = sample(idxs, 2)
                    f.write("Adjacent({}, {})".format(k, l))
                elif op == 4:
                    [k, l] = sample(idxs, 2)
                    f.write("AdjacentNot({}, {})".format(k, l))


if __name__ == "__main__":
    seed(428)
    for i in tqdm(range(5000)):
        golden_ass = gen_ass()
        gen_spec(golden_ass)
        with open(OUTPUT, "w") as f:
            run(["./seatAss", INPUT], stdout=f)
        status = check(INPUT, OUTPUT, golden_ass)
        if status != "CORRECT":
            print(status)
            gold_str = ""
            for i, num in enumerate(golden_ass):
                gold_str += "{}({}) ".format(i, num)
            print(gold_str)
            input()
    print(true_pos)