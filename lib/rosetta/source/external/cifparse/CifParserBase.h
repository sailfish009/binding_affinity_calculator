/*
FILE:     CifParserBase.h
*/
/*
VERSION:  7.105
*/
/*
DATE:     10/21/2013
*/
/*
  Comments and Questions to: sw-help@rcsb.rutgers.edu
*/
/*
COPYRIGHT 1999-2013 Rutgers - The State University of New Jersey

This software is provided WITHOUT WARRANTY OF MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE OR ANY OTHER WARRANTY, EXPRESS OR
IMPLIED.  RUTGERS MAKE NO REPRESENTATION OR WARRANTY THAT THE
SOFTWARE WILL NOT INFRINGE ANY PATENT, COPYRIGHT OR OTHER
PROPRIETARY RIGHT.

The user of this software shall indemnify, hold harmless and defend
Rutgers, its governors, trustees, officers, employees, students,
agents and the authors against any and all claims, suits,
losses, liabilities, damages, costs, fees, and expenses including
reasonable attorneys' fees resulting from or arising out of the
use of this software.  This indemnification shall include, but is
not limited to, any and all claims alleging products liability.
*/
/*
               RCSB PDB SOFTWARE LICENSE AGREEMENT

BY CLICKING THE ACCEPTANCE BUTTON OR INSTALLING OR USING 
THIS "SOFTWARE, THE INDIVIDUAL OR ENTITY LICENSING THE  
SOFTWARE ("LICENSEE") IS CONSENTING TO BE BOUND BY AND IS 
BECOMING A PARTY TO THIS AGREEMENT.  IF LICENSEE DOES NOT 
AGREE TO ALL OF THE TERMS OF THIS AGREEMENT
THE LICENSEE MUST NOT INSTALL OR USE THE SOFTWARE.

1. LICENSE AGREEMENT

This is a license between you ("Licensee") and the Protein Data Bank (PDB) 
at Rutgers, The State University of New Jersey (hereafter referred to 
as "RUTGERS").   The software is owned by RUTGERS and protected by 
copyright laws, and some elements are protected by laws governing 
trademarks, trade dress and trade secrets, and may be protected by 
patent laws. 

2. LICENSE GRANT

RUTGERS grants you, and you hereby accept, non-exclusive, royalty-free 
perpetual license to install, use, modify, prepare derivative works, 
incorporate into other computer software, and distribute in binary 
and source code format, or any derivative work thereof, together with 
any associated media, printed materials, and on-line or electronic 
documentation (if any) provided by RUTGERS (collectively, the "SOFTWARE"), 
subject to the following terms and conditions: (i) any distribution 
of the SOFTWARE shall bind the receiver to the terms and conditions 
of this Agreement; (ii) any distribution of the SOFTWARE in modified 
form shall clearly state that the SOFTWARE has been modified from 
the version originally obtained from RUTGERS.  

2. COPYRIGHT; RETENTION OF RIGHTS.  

The above license grant is conditioned on the following: (i) you must 
reproduce all copyright notices and other proprietary notices on any 
copies of the SOFTWARE and you must not remove such notices; (ii) in 
the event you compile the SOFTWARE, you will include the copyright 
notice with the binary in such a manner as to allow it to be easily 
viewable; (iii) if you incorporate the SOFTWARE into other code, you 
must provide notice that the code contains the SOFTWARE and include 
a copy of the copyright notices and other proprietary notices.  All 
copies of the SOFTWARE shall be subject to the terms of this Agreement.  

3. NO MAINTENANCE OR SUPPORT; TREATMENT OF ENHANCEMENTS 

RUTGERS is under no obligation whatsoever to: (i) provide maintenance 
or support for the SOFTWARE; or (ii) to notify you of bug fixes, patches, 
or upgrades to the features, functionality or performance of the 
SOFTWARE ("Enhancements") (if any), whether developed by RUTGERS 
or third parties.  If, in its sole discretion, RUTGERS makes an 
Enhancement available to you and RUTGERS does not separately enter 
into a written license agreement with you relating to such bug fix, 
patch or upgrade, then it shall be deemed incorporated into the SOFTWARE 
and subject to this Agreement. You are under no obligation whatsoever 
to provide any Enhancements to RUTGERS or the public that you may 
develop over time; however, if you choose to provide your Enhancements 
to RUTGERS, or if you choose to otherwise publish or distribute your 
Enhancements, in source code form without contemporaneously requiring 
end users or RUTGERS to enter into a separate written license agreement 
for such Enhancements, then you hereby grant RUTGERS a non-exclusive,
royalty-free perpetual license to install, use, modify, prepare
derivative works, incorporate into the SOFTWARE or other computer
software, distribute, and sublicense your Enhancements or derivative
works thereof, in binary and source code form.

4. FEES.  There is no license fee for the SOFTWARE.  If Licensee
wishes to receive the SOFTWARE on media, there may be a small charge
for the media and for shipping and handling.  Licensee is
responsible for any and all taxes.

5. TERMINATION.  Without prejudice to any other rights, Licensor
may terminate this Agreement if Licensee breaches any of its terms
and conditions.  Upon termination, Licensee shall destroy all
copies of the SOFTWARE.

6. PROPRIETARY RIGHTS.  Title, ownership rights, and intellectual
property rights in the Product shall remain with RUTGERS.  Licensee 
acknowledges such ownership and intellectual property rights and will 
not take any action to jeopardize, limit or interfere in any manner 
with RUTGERS' ownership of or rights with respect to the SOFTWARE.  
The SOFTWARE is protected by copyright and other intellectual 
property laws and by international treaties.  Title and related 
rights in the content accessed through the SOFTWARE is the property 
of the applicable content owner and is protected by applicable law.  
The license granted under this Agreement gives Licensee no rights to such
content.

7. DISCLAIMER OF WARRANTY.  THE SOFTWARE IS PROVIDED FREE OF 
CHARGE, AND, THEREFORE, ON AN "AS IS" BASIS, WITHOUT WARRANTY OF 
ANY KIND, INCLUDING WITHOUT LIMITATION THE WARRANTIES THAT IT 
IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE 
OR NON-INFRINGING.  THE ENTIRE RISK AS TO THE QUALITY AND 
PERFORMANCE OF THE SOFTWARE IS BORNE BY LICENSEE.  SHOULD THE 
SOFTWARE PROVE DEFECTIVE IN ANY RESPECT, THE LICENSEE AND NOT 
LICENSOR ASSUMES THE ENTIRE COST OF ANY SERVICE AND REPAIR.  
THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF 
THIS AGREEMENT.  NO USE OF THE PRODUCT IS AUTHORIZED HEREUNDER 
EXCEPT UNDER THIS DISCLAIMER.

8. LIMITATION OF LIABILITY.  TO THE MAXIMUM EXTENT PERMITTED BY
APPLICABLE LAW,  IN NO EVENT WILL LICENSOR BE LIABLE FOR ANY 
INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING 
OUT OF THE USE OF OR INABILITY TO USE THE SOFTWARE, INCLUDING, 
WITHOUT LIMITATION, DAMAGES FOR LOSS OF GOODWILL, WORK 
STOPPAGE, COMPUTER FAILURE OR MALFUNCTION, OR ANY AND ALL 
OTHER COMMERCIAL DAMAGES OR LOSSES, EVEN IF ADVISED OF THE
POSSIBILITY THEREOF. 
*/


/*!
** \file CifParserBase.h
**
** \brief Header file for CifParser class.
*/


#ifndef CIF_PARSER_BASE_H
#define CIF_PARSER_BASE_H


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ISTable.h"
#include "CifFile.h"
#include "CifScannerBase.h"
#include "CifParserInt.h"
#include "CifFileReadDef.h"


#define  DATA_TAG "data_"


/**
**  \class CifParser
**
**  \brief Public class that respresents a CIF parser.
**
**  This class represents a CIF parser. This class utilizes flex/bison for
**  syntax/semantic processing and stores the parsed data (data blocks and
**  tables) in a \e CifFile object.
*/
class CifParser : public CifScanner
{
    public:
        /**
        **  Constructs a CIF parser.
        **
        **  \param[in] cifFileP - pointer to the \e CifFile object that the
        **    CIF parser is to use to store the parsed data
        **  \param[in] verbose - optional parameter that indicates whether
        **    parsing logging should be turned on (if true) or off (if false).
        **    If \e verbose is not specified, logging is turned off.
        **
        **  \return Not applicable
        **
        **  \pre \e cifFileP must not be NULL
        **
        **  \post None
        **
        **  \exception EmptyValueException - if \e cifFileP is NULL
        */
        CifParser(CifFile* cifFileP, bool verbose = false);

        /**
        **  Method, not currently part of users public API, and will soon be
        **  re-examined.
        */
        CifParser(CifFile* cifFileP, CifFileReadDef readDef,
          bool verbose = false);

        /**
        **  Parses the CIF file.
        **
        **  \param[in] fileName - relative or absolute name of the CIF file
        **    that is to be parsed.
        **  \param[in] parseLogFileName - relative or absolute name of the file
        **    where parsing log is to be stored.
        **  \param[out] diagnostics - parsing result. If empty, parsing
        **    completed with no warnings or errors. If non-empty, there were
        **    parsing warnings and/or parsing errors.
        **
        **  \return None
        **
        **  \pre None
        **
        **  \post None
        **
        **  \exception: None
        */
        void Parse(const string& fileName, string& diagnostics,
          const std::string& parseLogFileName = std::string());

        /**
        **  Parses the CIF data in a string.
        **
        **  \param[in] cifString - a string that contains CIF data that is to
        **    be parsed.
        **  \param[out] diagnostics - parsing result. If empty, parsing
        **    completed with no warnings or errors. If non-empty, there were
        **    parsing warnings and/or parsing errors.
        **
        **  \return None
        **
        **  \pre None
        **
        **  \post None
        **
        **  \exception: None
        */
        void ParseString(const string& cifString, string& diagnostics);

        /**
        **  Destructs a CIF parser by releasing all the used resources.
        **
        **  \param: Not applicable
        **
        **  \return Not applicable
        **
        **  \pre None
        **
        **  \post None
        **
        **  \exception: None
        */
        virtual ~CifParser();

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void Error(const char*);

        /**
        **  Method, not currently part of users public API, and will soon be
        **  re-examined.
        */
        void Clear();

        /**
        **  Method, not currently part of users public API, and will soon be
        **  re-examined.
        */
        void Reset();

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        int ProcessLoopDeclaration(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        int ProcessItemNameList(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        int ProcessValueList(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        int ProcessItemValuePair(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessAssignments(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessLoop(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessItemName(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessItemValue(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessLsItemValue(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessUnknownValue(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessMissingValue(void);

        /**
        **  Utility method, not part of users public API, and will soon be
        **  removed.
        */
        void ProcessDataBlockName(void);

    private:
        CifFile *_fobj;
        CifFileReadDef _readDef;
        ISTable *_tbl;
        int _afterLoop;
        int _nTablesInBlock;
        int _curItemNo, _curValueNo, _numDataBlocks, _fieldListAlloc, _curRow;
        vector<string> _fieldList;
        string _pBufValue;
        string _tBufKeyword;
        string _curCategoryName;
        string _curDataBlockName;
        string _prevDataBlockName;
        void _ComplexWriteTable();

        int _err, _warn;
};
 
#endif /* CIF_PARSER_BASE_H */