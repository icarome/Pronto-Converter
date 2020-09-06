import sys
class Error(Exception):
    """Base class for other exceptions"""
    pass


class InvalidInput(Error):
    """Raised when the input value is too small"""
    pass
def pronto2arduino(codes):
    if len(codes)<70:
        raise InvalidInput
    i=0
    codes = codes[6:(64-len(codes[6:]))]
    bincode = []
    for i in range(len(codes)):
        if i == 0 or i%2==0:
            #print(i/2, codes[i], codes[i+1])
            bit = int(codes[i],16) + int(codes[i+1],16)
            if(bit < 50):
                bincode+=[str(0)]
            else:
                bincode+=[str(1)]
    return str(hex(int("".join(bincode),2))).upper().replace("X", "x")
print('\x1bc')
try:
    while True:
        code = raw_input('Novo Codigo --> ')
        print('\x1bc')
        print(pronto2arduino(code.split()))
except KeyboardInterrupt:
    print('\x1bc')
    exit()
except InvalidInput:
    print('\x1bc')
    print("Codigo mal formatado, tente novamente!")
    pass


