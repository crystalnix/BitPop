#!/usr/bin/python
#
# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Generator for Pnacl Shim functions that bridges the calling conventions
between GCC and PNaCl.  """

from datetime import datetime
import difflib
import glob
import os
import sys

from idl_c_proto import CGen
from idl_gen_wrapper import Interface, WrapperGen
from idl_log import ErrOut, InfoOut, WarnOut
from idl_option import GetOption, Option, ParseOptions
from idl_parser import ParseFiles

Option('pnaclshim', 'Name of the pnacl shim file.',
       default='temp_pnacl_shim.c')

Option('disable_pnacl_opt', 'Turn off optimization of pnacl shim.')


class PnaclGen(WrapperGen):
  """PnaclGen generates shim code to bridge the Gcc ABI with PNaCl.

  This subclass of WrapperGenerator takes the IDL sources and
  generates shim methods for bridging the calling conventions between GCC
  and PNaCl (LLVM). Some of the PPAPI methods do not need shimming, so
  this will also detect those situations and provide direct access to the
  original PPAPI methods (rather than the shim methods).
  """

  def __init__(self):
    WrapperGen.__init__(self,
                        'Pnacl',
                        'Pnacl Shim Gen',
                        'pnacl',
                        'Generate the PNaCl shim.')
    self.cgen = CGen()
    self._skip_opt = False
    self._pnacl_attribute = '__attribute__((pnaclcall))'

  ############################################################

  def OwnHeaderFile(self):
    """Return the header file that specifies the API of this wrapper.
    We do not generate the header files.  """
    return 'ppapi/generators/pnacl_shim.h'

  def InterfaceNeedsWrapper(self, iface, releases):
    """Return true if the interface has ANY methods that need wrapping.
    """
    if self._skip_opt:
      return True
    for release in iface.GetUniqueReleases(releases):
      version = iface.GetVersion(release)
      if self.InterfaceVersionNeedsWrapping(iface, version):
        return True
    return False


  def InterfaceVersionNeedsWrapping(self, iface, version):
    """Return true if the interface+version has ANY methods that
    need wrapping.
    """
    if self._skip_opt:
      return True
    for member in iface.GetListOf('Member'):
      release = member.GetRelease(version)
      if self.MemberNeedsWrapping(member, release):
        return True
    return False


  def MemberNeedsWrapping(self, member, release):
    """Return true if a particular member function at a particular
    release needs wrapping.
    """
    if self._skip_opt:
      return True
    if not member.InReleases([release]):
      return False
    ret, name, array, args_spec = self.cgen.GetComponents(member,
                                                          release,
                                                          'store')
    return self.TypeNeedsWrapping(ret, []) or self.ArgsNeedWrapping(args_spec)


  def ArgsNeedWrapping(self, args):
    """Return true if any parameter in the list needs wrapping.
    """
    for arg in args:
      (type_str, name, array_dims, more_args) = arg
      if self.TypeNeedsWrapping(type_str, array_dims):
        return True
    return False


  def TypeNeedsWrapping(self, type_node, array_dims):
    """Return true if a parameter type needs wrapping.
    Currently, this is true for byval aggregates.
    """
    is_aggregate = type_node.startswith('struct') or \
        type_node.startswith('union')
    is_reference = (type_node.find('*') != -1 or array_dims != [])
    return is_aggregate and not is_reference

  ############################################################


  def GenerateWrapperForPPBMethod(self, iface, member):
    result = []
    func_prefix = self.WrapperMethodPrefix(iface.node, iface.release)
    sig = self.cgen.GetSignature(member, iface.release, 'store',
                                 func_prefix, False)
    result.append('static %s\n%s {\n' % (self._pnacl_attribute, sig))
    result.append('  const struct %s *iface = %s.real_iface;\n' %
                  (iface.struct_name, self.GetWrapperInfoName(iface)))
    ret, name, array, cspec = self.cgen.GetComponents(member,
                                                      iface.release,
                                                      'store')
    ret_str, args_str = self.GetReturnArgs(ret, cspec)
    result.append('  %siface->%s(%s);\n}\n\n' % (ret_str,
                                                 member.GetName(), args_str))
    return result


  def GenerateWrapperForPPPMethod(self, iface, member):
    result = []
    func_prefix = self.WrapperMethodPrefix(iface.node, iface.release)
    sig = self.cgen.GetSignature(member, iface.release, 'store',
                                 func_prefix, False)
    result.append('static %s {\n' % sig)
    result.append('  const struct %s *iface = %s.real_iface;\n' %
                  (iface.struct_name, self.GetWrapperInfoName(iface)))
    temp_fp = self.cgen.GetSignature(member, iface.release, 'return',
                                     'temp_fp',
                                     func_as_ptr=True,
                                     ptr_prefix=self._pnacl_attribute + ' ',
                                     include_name=False)
    cast = self.cgen.GetSignature(member, iface.release, 'return',
                                  prefix='',
                                  func_as_ptr=True,
                                  ptr_prefix=self._pnacl_attribute + ' ',
                                  include_name=False)
    result.append('  %s = ((%s)iface->%s);\n' % (temp_fp,
                                                 cast,
                                                 member.GetName()))
    ret, name, array, cspec = self.cgen.GetComponents(member,
                                                      iface.release,
                                                      'store')
    ret_str, args_str = self.GetReturnArgs(ret, cspec)
    result.append('  %stemp_fp(%s);\n}\n\n' % (ret_str, args_str))
    return result


  def GenerateRange(self, ast, releases, options):
    """Generate shim code for a range of releases.
    """
    self._skip_opt = GetOption('disable_pnacl_opt')
    self.SetOutputFile(GetOption('pnaclshim'))
    return WrapperGen.GenerateRange(self, ast, releases, options)

pnaclgen = PnaclGen()

######################################################################
# Tests.

# Clean a string representing an object definition and return then string
# as a single space delimited set of tokens.
def CleanString(instr):
  instr = instr.strip()
  instr = instr.split()
  return ' '.join(instr)


def PrintErrorDiff(old, new):
  oldlines = old.split(';')
  newlines = new.split(';')
  d = difflib.Differ()
  diff = d.compare(oldlines, newlines)
  ErrOut.Log('Diff is:\n%s' % '\n'.join(diff))


def GetOldTestOutput(ast):
  # Scan the top-level comments in the IDL file for comparison.
  old = []
  for filenode in ast.GetListOf('File'):
    for node in filenode.GetChildren():
      instr = node.GetOneOf('Comment')
      if not instr: continue
      instr.Dump()
      old.append(instr.GetName())
  return CleanString(''.join(old))


def TestFiles(filenames, test_releases):
  ast = ParseFiles(filenames)
  iface_releases = pnaclgen.DetermineInterfaces(ast, test_releases)
  new_output = CleanString(pnaclgen.GenerateWrapperForMethods(
      iface_releases, comments=False))
  old_output = GetOldTestOutput(ast)
  if new_output != old_output:
    PrintErrorDiff(old_output, new_output)
    ErrOut.Log('Failed pnacl generator test.')
    return 1
  else:
    InfoOut.Log('Passed pnacl generator test.')
    return 0


def Main(args):
  filenames = ParseOptions(args)
  test_releases = ['M13', 'M14', 'M15']
  if not filenames:
    idldir = os.path.split(sys.argv[0])[0]
    idldir = os.path.join(idldir, 'test_gen_pnacl', '*.idl')
    filenames = glob.glob(idldir)
  filenames = sorted(filenames)
  if GetOption('test'):
    # Run the tests.
    return TestFiles(filenames, test_releases)

  # Otherwise, generate the output file (for potential use as golden file).
  ast = ParseFiles(filenames)
  return pnaclgen.GenerateRange(ast, test_releases, filenames)


if __name__ == '__main__':
  retval = Main(sys.argv[1:])
  sys.exit(retval)
