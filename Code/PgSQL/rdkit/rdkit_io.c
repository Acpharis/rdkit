// $Id$
//
//  Copyright (c) 2010, Novartis Institutes for BioMedical Research Inc.
//  All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met: 
//
//     * Redistributions of source code must retain the above copyright 
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following 
//       disclaimer in the documentation and/or other materials provided 
//       with the distribution.
//     * Neither the name of Novartis Institutes for BioMedical Research Inc. 
//       nor the names of its contributors may be used to endorse or promote 
//       products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "rdkit.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(mol_in);
Datum		mol_in(PG_FUNCTION_ARGS);
Datum
mol_in(PG_FUNCTION_ARGS) {
	char 	*data = PG_GETARG_CSTRING(0);
	CROMol	mol;
	Mol	*res;

	mol = parseMolText(data);

#ifdef REPACK
	/*
	 *  results of mol_in() and mol_in(mol_out(mol_in())) are different!
	 */
	do {
		char *str;
		int len;

		str = makeMolText(mol, &len);
		freeCROMol(mol);
		mol = parseMolText(str);
	} while(0);
#endif

	res = deconstructROMol(mol);
	freeCROMol(mol);

	PG_RETURN_MOL_P(res);		
}


PG_FUNCTION_INFO_V1(mol_out);
Datum		mol_out(PG_FUNCTION_ARGS);
Datum
mol_out(PG_FUNCTION_ARGS) {
	CROMol	mol;
	char	*str;
	int	len;

    fcinfo->flinfo->fn_extra = SearchMolCache(
									fcinfo->flinfo->fn_extra,
									fcinfo->flinfo->fn_mcxt,
									PG_GETARG_DATUM(0),
									NULL, &mol, NULL);
	str = makeMolText(mol, &len);

	PG_RETURN_CSTRING( pnstrdup(str, len) );
}

PG_FUNCTION_INFO_V1(bfp_in);
Datum		bfp_in(PG_FUNCTION_ARGS);
Datum
bfp_in(PG_FUNCTION_ARGS) {
	MolBitmapFingerPrint	fp;
	BitmapFingerPrint 	*b = DatumGetBitmapFingerPrintP(DirectFunctionCall1(
						byteain,
						PG_GETARG_DATUM(0)
					));

	/* check correctness */
	fp = constructMolBitmapFingerPrint(b);
	freeMolBitmapFingerPrint(fp);

	PG_RETURN_BITMAPFINGERPRINT_P(b);
}

PG_FUNCTION_INFO_V1(bfp_out);
Datum		bfp_out(PG_FUNCTION_ARGS);
Datum
bfp_out(PG_FUNCTION_ARGS) {
	PG_RETURN_DATUM( DirectFunctionCall1( byteaout, PG_GETARG_DATUM(0) ) );
}

PG_FUNCTION_INFO_V1(sfp_in);
Datum		sfp_in(PG_FUNCTION_ARGS);
Datum
sfp_in(PG_FUNCTION_ARGS) {
	MolSparseFingerPrint	fp;
	SparseFingerPrint 	*b = DatumGetSparseFingerPrintP(DirectFunctionCall1(
						byteain,
						PG_GETARG_DATUM(0)
					));

	/* check correctness */
	fp = constructMolSparseFingerPrint(b);
	freeMolSparseFingerPrint(fp);

	PG_RETURN_SPARSEFINGERPRINT_P(b);
}

PG_FUNCTION_INFO_V1(sfp_out);
Datum		sfp_out(PG_FUNCTION_ARGS);
Datum
sfp_out(PG_FUNCTION_ARGS) {
	PG_RETURN_DATUM( DirectFunctionCall1( byteaout, PG_GETARG_DATUM(0) ) );
}

PG_FUNCTION_INFO_V1(rdkit_version);
Datum		rdkit_version(PG_FUNCTION_ARGS);
Datum
rdkit_version(PG_FUNCTION_ARGS) {
	char	*ver = "" RDKITVER;
	char	buf[1024];
	Assert(strlen(ver) == 6);
	snprintf(buf, sizeof(buf), "%d.%d.%d", 
			atoi( pnstrdup(ver, 2) ),  
			atoi( pnstrdup(ver + 2 , 2) ),  
			atoi( pnstrdup(ver + 4, 2) ));

	PG_RETURN_TEXT_P(cstring_to_text(buf));
}
