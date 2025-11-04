def to_upper(c):
    if ord('a') <= ord(c) <= ord('z'):
        return chr(ord(c) + ord('A') - ord('a'))
    return c

def str_to_crc(string):
    if not string:
        return 0xFFFFFFFF  
    if isinstance(string, str):
        str_bytes = string.encode('ascii')
    else:
        str_bytes = string

    length = len(str_bytes)
    str_len = length
    a = 0x9e3779b9
    b = a
    c = 0

    idx = 0
    while length >= 12:
        a += (ord(to_upper(chr(str_bytes[idx+0]))) +
              (ord(to_upper(chr(str_bytes[idx+1]))) << 8) +
              (ord(to_upper(chr(str_bytes[idx+2]))) << 16) +
              (ord(to_upper(chr(str_bytes[idx+3]))) << 24))
        a &= 0xFFFFFFFF

        b += (ord(to_upper(chr(str_bytes[idx+4]))) +
              (ord(to_upper(chr(str_bytes[idx+5]))) << 8) +
              (ord(to_upper(chr(str_bytes[idx+6]))) << 16) +
              (ord(to_upper(chr(str_bytes[idx+7]))) << 24))
        b &= 0xFFFFFFFF

        c += (ord(to_upper(chr(str_bytes[idx+8]))) +
              (ord(to_upper(chr(str_bytes[idx+9]))) << 8) +
              (ord(to_upper(chr(str_bytes[idx+10]))) << 16) +
              (ord(to_upper(chr(str_bytes[idx+11]))) << 24))
        c &= 0xFFFFFFFF

        # Mix
        a = (a - b - c) & 0xFFFFFFFF
        a ^= (c >> 13)
        a &= 0xFFFFFFFF

        b = (b - c - a) & 0xFFFFFFFF
        b ^= (a << 8)
        b &= 0xFFFFFFFF

        c = (c - a - b) & 0xFFFFFFFF
        c ^= (b >> 13)
        c &= 0xFFFFFFFF

        a = (a - b - c) & 0xFFFFFFFF
        a ^= (c >> 12)
        a &= 0xFFFFFFFF

        b = (b - c - a) & 0xFFFFFFFF
        b ^= (a << 16)
        b &= 0xFFFFFFFF

        c = (c - a - b) & 0xFFFFFFFF
        c ^= (b >> 5)
        c &= 0xFFFFFFFF

        a = (a - b - c) & 0xFFFFFFFF
        a ^= (c >> 3)
        a &= 0xFFFFFFFF

        b = (b - c - a) & 0xFFFFFFFF
        b ^= (a << 10)
        b &= 0xFFFFFFFF

        c = (c - a - b) & 0xFFFFFFFF
        c ^= (b >> 15)
        c &= 0xFFFFFFFF

        idx += 12
        length -= 12

    # Handle the last 11 bytes
    c += str_len
    c &= 0xFFFFFFFF

    if length >= 11:
        c += (ord(to_upper(chr(str_bytes[idx+10]))) << 24)
        c &= 0xFFFFFFFF
    if length >= 10:
        c += (ord(to_upper(chr(str_bytes[idx+9]))) << 16)
        c &= 0xFFFFFFFF
    if length >= 9:
        c += (ord(to_upper(chr(str_bytes[idx+8]))) << 8)
        c &= 0xFFFFFFFF
    if length >= 8:
        b += (ord(to_upper(chr(str_bytes[idx+7]))) << 24)
        b &= 0xFFFFFFFF
    if length >= 7:
        b += (ord(to_upper(chr(str_bytes[idx+6]))) << 16)
        b &= 0xFFFFFFFF
    if length >= 6:
        b += (ord(to_upper(chr(str_bytes[idx+5]))) << 8)
        b &= 0xFFFFFFFF
    if length >= 5:
        b += ord(to_upper(chr(str_bytes[idx+4])))
        b &= 0xFFFFFFFF
    if length >= 4:
        a += (ord(to_upper(chr(str_bytes[idx+3]))) << 24)
        a &= 0xFFFFFFFF
    if length >= 3:
        a += (ord(to_upper(chr(str_bytes[idx+2]))) << 16)
        a &= 0xFFFFFFFF
    if length >= 2:
        a += (ord(to_upper(chr(str_bytes[idx+1]))) << 8)
        a &= 0xFFFFFFFF
    if length >= 1:
        a += ord(to_upper(chr(str_bytes[idx+0])))
        a &= 0xFFFFFFFF

    # Final mix
    a = (a - b - c) & 0xFFFFFFFF
    a ^= (c >> 13)
    a &= 0xFFFFFFFF

    b = (b - c - a) & 0xFFFFFFFF
    b ^= (a << 8)
    b &= 0xFFFFFFFF

    c = (c - a - b) & 0xFFFFFFFF
    c ^= (b >> 13)
    c &= 0xFFFFFFFF

    a = (a - b - c) & 0xFFFFFFFF
    a ^= (c >> 12)
    a &= 0xFFFFFFFF

    b = (b - c - a) & 0xFFFFFFFF
    b ^= (a << 16)
    b &= 0xFFFFFFFF

    c = (c - a - b) & 0xFFFFFFFF
    c ^= (b >> 5)
    c &= 0xFFFFFFFF

    a = (a - b - c) & 0xFFFFFFFF
    a ^= (c >> 3)
    a &= 0xFFFFFFFF

    b = (b - c - a) & 0xFFFFFFFF
    b ^= (a << 10)
    b &= 0xFFFFFFFF

    c = (c - a - b) & 0xFFFFFFFF
    c ^= (b >> 15)
    c &= 0xFFFFFFFF

    return c


def main():
    import sys

    if len(sys.argv) > 1:
        # Batch mode: process all arguments
        for arg in sys.argv[1:]:
            crc = str_to_crc(arg)
            print(f"{arg} -> {crc}")
            print()
    else:
        # Interactive mode
        print("UbiArt Framework StringID CRC Generator")
        print("=" * 50)
        print("Enter string to generate CRC (or 'quit' to exit)")
        print()

        while True:
            try:
                user_input = input("String: ").strip()

                if user_input.lower() in ['quit', 'exit', 'q']:
                    break

                if not user_input:
                    continue

                crc = str_to_crc(user_input)
                print(f"  CRC Value: {crc}")
                print()

            except KeyboardInterrupt:
                print("\nExiting...")
                break
            except Exception as e:
                print(f"Error: {e}")


if __name__ == "__main__":
    main()
