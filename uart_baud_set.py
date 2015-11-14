import sys

if len(sys.argv) < 3:
    print("Usage: uart_baud_set.py pclk baudrate")
    sys.exit(1)

pclk = float(sys.argv[1])
baudrate = float(sys.argv[2])

def calc_rate(param):
    divisor = 256.0 * param[0] + param[1] if (param[0] != 0) or (param[1] != 0) else 1.0
    return pclk / (16.0 * divisor * (1.0 + param[3] / param[2]))

candidates = []
for u0dlm in range(0, 256):
    for u0dll in range(0, 256):
        for mulval in range(1, 16):
            for divaddval in range(0, mulval):
                candidates.append((u0dlm, u0dll, mulval, divaddval))

candidates.sort(key = lambda x : abs(calc_rate(x) - baudrate))

print("U0DLM\tU0DLL\tMULVAL\tDIVADDVAL\tbaudrate")
for i in range(10):
    print("\t".join(map(str, candidates[i])) + "\t" + str(calc_rate(candidates[i])))
