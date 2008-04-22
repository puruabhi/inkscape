#ifndef __DIGEST_H__
#define __DIGEST_H__
/**
 * Secure Hashing Tool
 * *
 * Author:
 *   Bob Jamison
 *
 * Copyright (C) 2006-2008 Bob Jamison
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 *
 *  This base class and its subclasses provide an easy API for providing
 *  several different types of secure hashing functions for whatever use
 *  a developer might need.  This is not intended as a high-performance
 *  replacement for the fine implementations already available.  Rather, it
 *  is a small and simple (and maybe a bit slow?) tool for moderate common
 *  hashing requirements, like for communications and authentication.
 *
 *  These hashes are intended to be simple to use.  For example:
 *  Sha256 digest;
 *  digest.append("The quick brown dog");
 *  std::string result = digest.finishHex();
 *
 *  Or, use one of the static convenience methods:
 *
 *   example:  std::string digest =
 *                     Digest::hash(Digest::HASH_XXX, str);
 *
 *    ...where HASH_XXX represents one of the hash
 *       algorithms listed in HashType.
 *
 *  There are several forms of append() for convenience.
 *  finish() and finishHex() call reset() for both security and
 *  to prepare for the next use.
 *
 *
 *  Much effort has been made to make this code portable, and it
 *  has been tested on various 32- and 64-bit machines.  If you
 *  add another algorithm, please test it likewise.
 *
 *
 *  The SHA algorithms are derived directly from FIPS-180-3.  The
 *  SHA tests at the bottom are also directly from that document.
 *
 *  The MD5 algorithm is from RFC 1321
 *
 */

#include <vector>
#include <string>


/**
 *  Base class.  Do not use this class directly.  Rather, use of of the
 *  subclasses below.
 *  For all subclasses, overload reset(), update(unsigned char),
 *  transform(), and finish()
 */
class Digest
{
public:

    /**
     *  Different types of hash algorithms.
     */
    typedef enum
        {
        HASH_NONE,
        HASH_SHA1,
        HASH_SHA224,
        HASH_SHA256,
        HASH_SHA384,
        HASH_SHA512,
        HASH_MD5
        } HashType;

    /**
     *  Constructor, with no type
     */
    Digest() : hashType(HASH_NONE)
        { reset(); }

    /**
     *  Destructor
     */
    virtual ~Digest()
        { reset(); }

    /**
     *  Return one of the enumerated hash types above
     */
    virtual int getType()
        { return hashType; }

    /**
     *  Append a single byte to the hash
     */
    void append(unsigned char ch)
        { update(ch); }

    /**
     *  Append a string to the hash
     */
    virtual void append(const std::string &str)
        {
        for (unsigned int i=0 ; i<str.size() ; i++)
            update((unsigned char)str[i]);
        }

    /**
     *  Append a byte buffer to the hash
     */
    virtual void append(unsigned char *buf, int len)
        {
        for (int i=0 ; i<len ; i++)
            update(buf[i]);
        }

    /**
     *  Append a byte vector to the hash
     */
    virtual void append(const std::vector<unsigned char> buf)
        {
        for (unsigned int i=0 ; i<buf.size() ; i++)
            update(buf[i]);
        }

    /**
     *  Finish the hash and return a hexidecimal version of the computed
     *  value
     */
    virtual std::string finishHex();

    /**
     *  Initialize the fields of this hash engine to its starting values.
     *  Overload this in every subclass
     */
    virtual void reset()
        { clearByteCount(); }

    /**
     *  Finish the hash and return its computed value
     *  Overload this in every subclass
     */
    virtual std::vector<unsigned char> finish()
        {
        std::vector<unsigned char> ret;
        return ret;
        }


    //########################
    //# Convenience methods
    //########################

    /**
     * Convenience method.  This is a simple way of getting a hash.
     * call with:  std::vector<unsigned char> digest =
     *             Digest::hash(Digest::HASH_XXX, buf, len);
     */
    static std::vector<unsigned char> hash(HashType typ,
                                           unsigned char *buf,
                                           int len);
    /**
     * Convenience method.  This is a simple way of getting a hash.
     * call with:  std::vector<unsigned char> digest =
     *             Digest::hash(Digest::HASH_XXX, str);
     */
    static std::vector<unsigned char> hash(HashType typ,
                                           const std::string &str);

    /**
     * Convenience method.  This is a simple way of getting a hash.
     * call with:  std::string digest =
     *             Digest::hash(Digest::HASH_XXX, buf, len);
     */
    static std::string hashHex(HashType typ,
                               unsigned char *buf,
                               int len);
    /**
     * Convenience method.  This is a simple way of getting a hash.
     * call with:  std::string digest =
     *             Digest::hash(Digest::HASH_XXX, str);
     */
    static std::string hashHex(HashType typ,
                               const std::string &str);

protected:

    /**
     *  Update the hash with a given byte
     *  Overload this in every subclass
     */
    virtual void update(unsigned char /*ch*/)
        {}

    /**
     *  Perform the particular block hashing algorithm for a
     *  particular type of hash.
     *  Overload this in every subclass
     */
    virtual void transform()
        {}


    /**
     * The enumerated type of the hash
     */
    int hashType;

    void incByteCount()
        {
        if (nrBytesLo == 0xffffffffL)
            {
            nrBytesHi++;
            nrBytesLo = 0;
            }
        else
           nrBytesLo++;
        }
    void clearByteCount()
        {
        nrBytesHi = nrBytesLo = 0;
        }
    void getBitCount()
        {
        nrBitsLo = (nrBytesLo << 3) & 0xffffffff;
        nrBitsHi = (nrBytesHi << 3) | ((nrBytesLo >> 29) & 7);
        }
    void appendBitCount()
        {
        update((unsigned char)((nrBitsHi>>24) & 0xff));
        update((unsigned char)((nrBitsHi>>16) & 0xff));
        update((unsigned char)((nrBitsHi>> 8) & 0xff));
        update((unsigned char)((nrBitsHi    ) & 0xff));
        update((unsigned char)((nrBitsLo>>24) & 0xff));
        update((unsigned char)((nrBitsLo>>16) & 0xff));
        update((unsigned char)((nrBitsLo>> 8) & 0xff));
        update((unsigned char)((nrBitsLo    ) & 0xff));
        }

    unsigned long nrBytesHi;
    unsigned long nrBytesLo;
    unsigned long nrBitsHi;
    unsigned long nrBitsLo;
};





/**
 *  SHA-1,
 *  Section 6.1, SECURE HASH STANDARD
 *  Federal Information Processing Standards Publication 180-2
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
 */
class Sha1 : public Digest
{
public:

    /**
     *  Constructor
     */
    Sha1()
        { hashType = HASH_SHA1; reset(); }

    /**
     *  Destructor
     */
    virtual ~Sha1()
        { reset(); }

    /**
     *  Overloaded from Digest
     */
    virtual void reset();

    /**
     *  Overloaded from Digest
     */
    virtual std::vector<unsigned char> finish();

protected:

    /**
     *  Overloaded from Digest
     */
    virtual void update(unsigned char val);

    /**
     *  Overloaded from Digest
     */
    virtual void transform();

private:

    unsigned long hashBuf[5];
    unsigned long inBuf[80];

    int longNr;
    int byteNr;
    unsigned long inb[4];

};






/**
 *  SHA-224,
 *  Section 6.1, SECURE HASH STANDARD
 *  Federal Information Processing Standards Publication 180-2
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
 */
class Sha224 : public Digest
{
public:

    /**
     *  Constructor
     */
    Sha224()
        { hashType = HASH_SHA224; reset(); }

    /**
     *  Destructor
     */
    virtual ~Sha224()
        { reset(); }

    /**
     *  Overloaded from Digest
     */
    virtual void reset();

    /**
     *  Overloaded from Digest
     */
    virtual std::vector<unsigned char> finish();

protected:

    /**
     *  Overloaded from Digest
     */
    virtual void update(unsigned char val);

    /**
     *  Overloaded from Digest
     */
    virtual void transform();

private:

    unsigned long hashBuf[8];
    unsigned long inBuf[64];
    int longNr;
    int byteNr;
    unsigned long inb[4];

};



/**
 *  SHA-256,
 *  Section 6.1, SECURE HASH STANDARD
 *  Federal Information Processing Standards Publication 180-2
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
 */
class Sha256 : public Digest
{
public:

    /**
     *  Constructor
     */
    Sha256()
        { hashType = HASH_SHA256; reset(); }

    /**
     *  Destructor
     */
    virtual ~Sha256()
        { reset(); }

    /**
     *  Overloaded from Digest
     */
    virtual void reset();

    /**
     *  Overloaded from Digest
     */
    virtual std::vector<unsigned char> finish();

protected:

    /**
     *  Overloaded from Digest
     */
    virtual void update(unsigned char val);

    /**
     *  Overloaded from Digest
     */
    virtual void transform();

private:

    unsigned long hashBuf[8];
    unsigned long inBuf[64];
    int longNr;
    int byteNr;
    unsigned long inb[4];

};



/**
 *  SHA-384,
 *  Section 6.1, SECURE HASH STANDARD
 *  Federal Information Processing Standards Publication 180-2
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
 */
class Sha384 : public Digest
{
public:

    /**
     *  Constructor
     */
    Sha384()
        { hashType = HASH_SHA384; reset(); }

    /**
     *  Destructor
     */
    virtual ~Sha384()
        { reset(); }

    /**
     *  Overloaded from Digest
     */
    virtual void reset();

    /**
     *  Overloaded from Digest
     */
    virtual std::vector<unsigned char> finish();

protected:

    /**
     *  Overloaded from Digest
     */
    virtual void update(unsigned char val);

    /**
     *  Overloaded from Digest
     */
    virtual void transform();



private:

    unsigned long long hashBuf[8];
    unsigned long long inBuf[80];
    int longNr;
    int byteNr;
    unsigned long long inb[8];

};




/**
 *  SHA-512,
 *  Section 6.1, SECURE HASH STANDARD
 *  Federal Information Processing Standards Publication 180-2
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
 */
class Sha512 : public Digest
{
public:

    /**
     *  Constructor
     */
    Sha512()
        { hashType = HASH_SHA512; reset(); }

    /**
     *  Destructor
     */
    virtual ~Sha512()
        { reset(); }

    /**
     *  Overloaded from Digest
     */
    virtual void reset();

    /**
     *  Overloaded from Digest
     */
    virtual std::vector<unsigned char> finish();

protected:

    /**
     *  Overloaded from Digest
     */
    virtual void update(unsigned char val);

    /**
     *  Overloaded from Digest
     */
    virtual void transform();

private:

    unsigned long long hashBuf[8];
    unsigned long long inBuf[80];
    int longNr;
    int byteNr;
    unsigned long long inb[8];

};









/**
 * IETF RFC 1321, MD5 Specification
 * http://www.ietf.org/rfc/rfc1321.txt
 */
class Md5 :  public Digest
{
public:

    /**
     *  Constructor
     */
    Md5()
        { hashType = HASH_MD5; reset(); }

    /**
     *  Destructor
     */
    virtual ~Md5()
        { reset(); }

    /**
     *  Overloaded from Digest
     */
    virtual void reset();

    /**
     *  Overloaded from Digest
     */
    virtual std::vector<unsigned char> finish();

protected:

    /**
     *  Overloaded from Digest
     */
    virtual void update(unsigned char val);

    /**
     *  Overloaded from Digest
     */
    virtual void transform();

private:

    unsigned long hashBuf[4];
    unsigned long inBuf[16];

    unsigned long inb[4];  // Buffer for input bytes as longs
    int           byteNr;  // which byte in long
    int           longNr;  // which long in 16-long buffer

};









#endif /*  __DIGEST_H__ */


