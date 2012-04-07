// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "crypto/ec_signature_creator.h"

#include <cryptohi.h>
#include <pk11pub.h>
#include <secerr.h>
#include <sechash.h>

#include "base/logging.h"
#include "crypto/ec_private_key.h"
#include "crypto/nss_util.h"
#include "crypto/scoped_nss_types.h"

namespace crypto {

namespace {

SECStatus SignData(SECItem* result,
                   SECItem* input,
                   SECKEYPrivateKey* key,
                   HASH_HashType hash_type) {
  if (key->keyType != ecKey) {
    DLOG(FATAL) << "Should be using an EC key.";
    PORT_SetError(SEC_ERROR_INVALID_ARGS);
    return SECFailure;
  }

  // Hash the input.
  std::vector<uint8> hash_data(HASH_ResultLen(hash_type));
  SECStatus rv = HASH_HashBuf(
      hash_type, &hash_data[0], input->data, input->len);
  if (rv != SECSuccess)
    return rv;
  SECItem hash = {siBuffer, &hash_data[0], hash_data.size()};

  // Compute signature of hash.
  int signature_len = PK11_SignatureLen(key);
  std::vector<uint8> signature_data(signature_len);
  SECItem sig = {siBuffer, &signature_data[0], signature_len};
  rv = PK11_Sign(key, &sig, &hash);
  if (rv != SECSuccess)
    return rv;

  // DER encode the signature.
  return DSAU_EncodeDerSigWithLen(result, &sig, sig.len);
}

}  // namespace

// static
ECSignatureCreator* ECSignatureCreator::Create(ECPrivateKey* key) {
  return new ECSignatureCreator(key);
}

ECSignatureCreator::ECSignatureCreator(ECPrivateKey* key)
    : key_(key) {
  EnsureNSSInit();
}

ECSignatureCreator::~ECSignatureCreator() { }

bool ECSignatureCreator::Sign(const uint8* data,
                              int data_len,
                              std::vector<uint8>* signature) {
  // Data to be signed
  SECItem secret;
  secret.type = siBuffer;
  secret.len = data_len;
  secret.data = const_cast<unsigned char*>(data);

  // SECItem to receive the output buffer.
  SECItem result;
  result.type = siBuffer;
  result.len = 0;
  result.data = NULL;

  // Sign the secret data and save it to |result|.
  SECStatus rv =
      SignData(&result, &secret, key_->key(), HASH_AlgSHA1);
  if (rv != SECSuccess) {
    DLOG(ERROR) << "DerSignData: " << PORT_GetError();
    return false;
  }

  // Copy the signed data into the output vector.
  signature->assign(result.data, result.data + result.len);
  SECITEM_FreeItem(&result, PR_FALSE /* only free |result.data| */);
  return true;
}

}  // namespace crypto
