import sys, unittest
from tools import SamplesTestCase

OUTPUT_DIAMOND_TOPO = '''\
Topological sort of func:
  AA
  BB
  CC
  DD
'''

OUTPUT_DIAMOND_PO = '''\
Basic blocks of func in post-order:
  DD
  CC
  BB
  AA
'''

OUTPUT_LOOPY_TOPO = '''\
Topological sort of func:
  Detected cycle: edge from BB4 to BB3
  Sorting failed
'''

OUTPUT_LOOPY_SCC = '''\
SCCs for func in post-order:
  SCC: DD
  SCC: CC2  CC1  CC
  SCC: BB4  BB3
  SCC: BB2  BB1  BB
  SCC: AA
'''

PROG = 'bb_toposort_sccs'

class TestBBToposortSCCS(SamplesTestCase):
    def test_topo_on_diamond(self):
        self.assertSampleOutput([PROG, '-topo'], 'diamond-cfg.ll', OUTPUT_DIAMOND_TOPO)

    def test_po_on_diamond(self):
        self.assertSampleOutput([PROG, '-po'], 'diamond-cfg.ll', OUTPUT_DIAMOND_PO)

    def test_topo_on_loopy(self):
        self.assertSampleOutput([PROG, '-topo'], 'loopy-cfg.ll', OUTPUT_LOOPY_TOPO)

    def test_scc_on_loopy(self):
        self.assertSampleOutput([PROG, '-scc'], 'loopy-cfg.ll', OUTPUT_LOOPY_SCC)

if __name__ == '__main__':
  unittest.main()
