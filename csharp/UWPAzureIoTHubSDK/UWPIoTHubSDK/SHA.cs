//  
//  By Elze Kool see http://www.microframework.nl/2009/09/05/shahmac-digest-class/ 
//   
namespace IoTHubAmqp
{
    using System;

    /// <summary> 
    ///   Static class providing Secure Hashing Algorithm (SHA-1, SHA-224, SHA-256) 
    /// </summary> 
    public static class SHA
    {
        // Number used in SHA256 hash function 
        static readonly uint[] sha256_k = new uint[] 
                                { 
                                    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 
                                    0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 
                                    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 
                                    0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 
                                    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 
                                    0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 
                                    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 
                                    0xc67178f2 
                                };


        // Rotate bits right 
        static uint rotateright(uint x, int n)
        {
            return ((x >> n) | (x << (32 - n)));
        }

        // Convert 4 bytes to big endian uint32 
        static uint big_endian_from_bytes(byte[] input, uint start)
        {
            uint r = 0;
            r |= (((uint)input[start]) << 24);
            r |= (((uint)input[start + 1]) << 16);
            r |= (((uint)input[start + 2]) << 8);
            r |= (((uint)input[start + 3]));
            return r;
        }

        // Convert big endian uint32 to bytes 
        static void bytes_from_big_endian(uint input, ref byte[] output, int start)
        {
            output[start] = (byte)((input & 0xFF000000) >> 24);
            output[start + 1] = (byte)((input & 0x00FF0000) >> 16);
            output[start + 2] = (byte)((input & 0x0000FF00) >> 8);
            output[start + 3] = (byte)((input & 0x000000FF));
        }

        // SHA-224/SHA-256 choice function 
        static uint choice(uint x, uint y, uint z)
        {
            return ((x & y) ^ (~x & z));
        }

        // SHA-224/SHA-256 majority function 
        static uint majority(uint x, uint y, uint z)
        {
            return ((x & y) ^ (x & z) ^ (y & z));
        }

        /// <summary> 
        ///   Compute HMAC SHA-256 
        /// </summary> 
        /// <param name = "secret">Secret</param> 
        /// <param name = "value">Password</param> 
        /// <returns>32 byte HMAC_SHA256</returns> 
        public static byte[] computeHMAC_SHA256(byte[] secret, byte[] value)
        {
            // Create two arrays, bi and bo 
            var bi = new byte[64 + value.Length];
            var bo = new byte[64 + 32];

            // Copy secret to both arrays 
            Array.Copy(secret, bi, secret.Length);
            Array.Copy(secret, bo, secret.Length);

            for (var i = 0; i < 64; i++)
            {
                // Xor bi with 0x36 
                bi[i] = (byte)(bi[i] ^ 0x36);

                // Xor bo with 0x5c 
                bo[i] = (byte)(bo[i] ^ 0x5c);
            }

            // Append value to bi 
            Array.Copy(value, 0, bi, 64, value.Length);

            // Append SHA256(bi) to bo 
            var sha_bi = computeSHA256(bi);
            Array.Copy(sha_bi, 0, bo, 64, 32);

            // Return SHA256(bo) 
            return computeSHA256(bo);
        }

        /// <summary> 
        ///   Compute SHA-256 digest 
        /// </summary> 
        /// <param name = "input">Input array</param> 
        public static byte[] computeSHA256(byte[] input)
        {
            // Initialize working parameters 
            uint a, b, c, d, e, f, g, h, i, s0, s1, t1, t2;
            uint h0 = 0x6a09e667;
            var h1 = 0xbb67ae85;
            uint h2 = 0x3c6ef372;
            var h3 = 0xa54ff53a;
            uint h4 = 0x510e527f;
            var h5 = 0x9b05688c;
            uint h6 = 0x1f83d9ab;
            uint h7 = 0x5be0cd19;
            uint blockstart = 0;

            // Calculate how long the padded message should be 
            var newinputlength = input.Length + 1;
            while ((newinputlength % 64) != 56) // length mod 512bits = 448bits 
            {
                newinputlength++;
            }

            // Create array for padded data 
            var processed = new byte[newinputlength + 8];
            Array.Copy(input, processed, input.Length);

            // Pad data with an 1 
            processed[input.Length] = 0x80;

            // Pad data with big endian 64bit length of message 
            // We do only 32 bits becouse input.length is 32 bit 
            processed[processed.Length - 4] = (byte)(((input.Length * 8) & 0xFF000000) >> 24);
            processed[processed.Length - 3] = (byte)(((input.Length * 8) & 0x00FF0000) >> 16);
            processed[processed.Length - 2] = (byte)(((input.Length * 8) & 0x0000FF00) >> 8);
            processed[processed.Length - 1] = (byte)(((input.Length * 8) & 0x000000FF));

            // Block of 32 bits values used in calculations 
            var wordblock = new uint[64];

            // Now process each 512 bit block 
            while (blockstart < processed.Length)
            {
                // break chunk into sixteen 32-bit big-endian words  
                for (i = 0; i < 16; i++)
                    wordblock[i] = big_endian_from_bytes(processed, blockstart + (i * 4));

                // Extend the sixteen 32-bit words into sixty-four 32-bit words: 
                for (i = 16; i < 64; i++)
                {
                    s0 = rotateright(wordblock[i - 15], 7) ^ rotateright(wordblock[i - 15], 18) ^ (wordblock[i - 15] >> 3);
                    s1 = rotateright(wordblock[i - 2], 17) ^ rotateright(wordblock[i - 2], 19) ^ (wordblock[i - 2] >> 10);
                    wordblock[i] = wordblock[i - 16] + s0 + wordblock[i - 7] + s1;
                }

                // Initialize hash value for this chunk: 
                a = h0;
                b = h1;
                c = h2;
                d = h3;
                e = h4;
                f = h5;
                g = h6;
                h = h7;

                // Main loop 
                for (i = 0; i < 64; i++)
                {
                    t1 = h + (rotateright(e, 6) ^ rotateright(e, 11) ^ rotateright(e, 25)) + choice(e, f, g) + sha256_k[i] + wordblock[i];
                    t2 = (rotateright(a, 2) ^ rotateright(a, 13) ^ rotateright(a, 22)) + majority(a, b, c);
                    h = g;
                    g = f;
                    f = e;
                    e = d + t1;
                    d = c;
                    c = b;
                    b = a;
                    a = t1 + t2;
                }

                // Add this chunk's hash to result so far 
                h0 += a;
                h1 += b;
                h2 += c;
                h3 += d;
                h4 += e;
                h5 += f;
                h6 += g;
                h7 += h;

                // Process next 512bit block 
                blockstart += 64;
            }

            // Prepare output 
            var output = new byte[32];
            bytes_from_big_endian(h0, ref output, 0);
            bytes_from_big_endian(h1, ref output, 4);
            bytes_from_big_endian(h2, ref output, 8);
            bytes_from_big_endian(h3, ref output, 12);
            bytes_from_big_endian(h4, ref output, 16);
            bytes_from_big_endian(h5, ref output, 20);
            bytes_from_big_endian(h6, ref output, 24);
            bytes_from_big_endian(h7, ref output, 28);

            return output;
        }
    }
}