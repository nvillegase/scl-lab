import sys
import ctypes

if __name__ == '__main__':

    party_id = int(sys.argv[1])
    n = int(sys.argv[2])
    t = int(sys.argv[3])
    conf = str(sys.argv[4])
    
    # -- Protocols --

    # Sample Share and Distribute Protocol
    # uint64_t SSNDExec(int local_party_id, int sending_party_id, int n, int t, char* conf);
    sclssnd = ctypes.CDLL('./ssnd/build/libsclssnd.so')
    ssnd = sclssnd.SSNDExec
    ssnd.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_uint64)
    ssnd.restype = ctypes.c_int64
    
    # BGW protocol
    # uint64_t BGWExec(int local_party_id, int n, int t, char* conf, uint64_t x_share_t, uint64_t y_share_t);
    sclbgw = ctypes.CDLL('./bgw/build/libsclbgw.so')
    bgw = sclbgw.BGWExec
    bgw.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_uint64, ctypes.c_uint64)
    bgw.restype = ctypes.c_int64
    
    # Shamir reconstruct
    # uint64_t SRSQExec(int local_party_id, uint64_t share, int t, int n, char* conf);
    sclsrsq = ctypes.CDLL('./srsq/build/libsclsrsq.so')
    srsq = sclsrsq.SRSQExec
    srsq.argtypes = (ctypes.c_int, ctypes.c_uint64, ctypes.c_int, ctypes.c_int, ctypes.c_char_p)
    srsq.restype = ctypes.c_int64
    
    # -- Protocols END --
    
    if party_id in [0, 1]:
        while True:
            secret_value = int(input("Input value to multiply: "))
            if not (0 <= secret_value <= 2**61 - 1):
                print("Number out of range. Try again...")
                continue
            print(f"Value in hexadecimal form: 0x{secret_value:0x}")
            break
    else:
        secret_value = 0

    share_x: int = ssnd(party_id, 0, n, t, conf.encode(), secret_value)
    share_y: int = ssnd(party_id, 1, n, t, conf.encode(), secret_value)
    print(f"Share of x, degree t: 0x{share_x:0x}")
    print(f"Share of y, degree t: 0x{share_y:0x}")
    
    share_xy: int = bgw(party_id, n, t, conf.encode(), share_x, share_y)
    print(f"Share of x*y, degree t: 0x{share_xy:0x}")
    
    xy: int = srsq(party_id, share_xy, t, n, conf.encode())
    print(f"x*y reconstructed (hex): 0x{xy:0x}")
    print(f"x*y reconstructed (dec): {xy}")
    
