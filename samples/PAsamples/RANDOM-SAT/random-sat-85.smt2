(set-logic QF_SLAH)

(declare-datatypes ((data_t 0)) (((c_data_t (sz Int)))))

(declare-heap (Int data_t))

(define-fun hck ((?x Int) (?y Int) (?v Int)) Bool
	(and
		(>= (- ?y ?x) 2) (<= (- ?y ?x) ?v)
		(sep (pto ?x (c_data_t (- ?y ?x))) (blk (+ ?x 1) ?y))
))

(define-fun-rec hls ((?x Int) (?y Int) (?v Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int)) (sep (hck ?x ?w ?v) (hls ?w ?y ?v)))
))

(declare-const x0 Int)
(declare-const x1 Int)
(declare-const v0 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const v2 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const v4 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)
(declare-const x21 Int)
(declare-const x22 Int)
(declare-const x23 Int)
(declare-const v22 Int)
(declare-const x24 Int)
(declare-const x25 Int)
(declare-const x26 Int)
(declare-const x27 Int)
(declare-const x28 Int)
(declare-const x29 Int)
(declare-const x30 Int)
(declare-const x31 Int)
(declare-const v30 Int)
(declare-const x32 Int)
(declare-const x33 Int)
(declare-const x34 Int)
(declare-const x35 Int)
(declare-const x36 Int)
(declare-const x37 Int)
(declare-const v36 Int)
(declare-const x38 Int)
(declare-const x39 Int)
(declare-const x40 Int)
(declare-const x41 Int)
(declare-const v40 Int)
(declare-const x42 Int)
(declare-const x43 Int)
(declare-const v42 Int)
(declare-const x44 Int)
(declare-const x45 Int)
(declare-const v44 Int)
(declare-const x46 Int)
(declare-const x47 Int)
(declare-const x48 Int)
(declare-const x49 Int)
(declare-const x50 Int)
(declare-const x51 Int)
(declare-const x52 Int)
(declare-const x53 Int)
(declare-const v52 Int)
(declare-const x54 Int)
(declare-const x55 Int)
(declare-const x56 Int)
(declare-const x57 Int)
(declare-const x58 Int)
(declare-const x59 Int)
(declare-const x60 Int)
(declare-const x61 Int)
(declare-const v60 Int)
(declare-const x62 Int)
(declare-const x63 Int)
(declare-const v62 Int)
(declare-const x64 Int)
(declare-const x65 Int)
(declare-const x66 Int)
(declare-const x67 Int)
(declare-const v66 Int)
(declare-const x68 Int)
(declare-const x69 Int)
(declare-const x70 Int)
(declare-const x71 Int)
(declare-const v70 Int)
(declare-const x72 Int)
(declare-const x73 Int)
(declare-const x74 Int)
(declare-const x75 Int)
(declare-const v74 Int)
(declare-const x76 Int)
(declare-const x77 Int)
(declare-const v76 Int)
(declare-const x78 Int)
(declare-const x79 Int)
(declare-const x80 Int)
(declare-const x81 Int)
(declare-const v80 Int)
(declare-const x82 Int)
(declare-const x83 Int)
(declare-const v82 Int)
(declare-const x84 Int)
(declare-const x85 Int)
(declare-const v84 Int)
(declare-const x86 Int)
(declare-const x87 Int)
(declare-const v86 Int)
(declare-const x88 Int)
(declare-const x89 Int)
(declare-const v88 Int)
(declare-const x90 Int)
(declare-const x91 Int)
(declare-const x92 Int)
(declare-const x93 Int)
(declare-const x94 Int)
(declare-const x95 Int)
(declare-const x96 Int)
(declare-const x97 Int)
(declare-const x98 Int)
(declare-const x99 Int)
(declare-const v98 Int)
(declare-const x100 Int)
(declare-const x101 Int)
(declare-const x102 Int)
(declare-const x103 Int)
(declare-const x104 Int)
(declare-const x105 Int)
(declare-const x106 Int)
(declare-const x107 Int)
(declare-const v106 Int)
(declare-const x108 Int)
(declare-const x109 Int)
(declare-const x110 Int)
(declare-const x111 Int)
(declare-const x112 Int)
(declare-const x113 Int)
(declare-const x114 Int)
(declare-const x115 Int)
(declare-const v114 Int)
(declare-const x116 Int)
(declare-const x117 Int)
(declare-const x118 Int)
(declare-const x119 Int)
(declare-const v118 Int)
(declare-const x120 Int)
(declare-const x121 Int)
(declare-const v120 Int)
(declare-const x122 Int)
(declare-const x123 Int)
(declare-const x124 Int)
(declare-const x125 Int)
(declare-const x126 Int)
(declare-const x127 Int)
(declare-const v126 Int)
(declare-const x128 Int)
(declare-const x129 Int)
(declare-const v128 Int)
(declare-const x130 Int)
(declare-const x131 Int)
(declare-const v130 Int)
(declare-const x132 Int)
(declare-const x133 Int)
(declare-const x134 Int)
(declare-const x135 Int)
(declare-const x136 Int)
(declare-const x137 Int)
(declare-const x138 Int)
(declare-const x139 Int)
(declare-const x140 Int)
(declare-const x141 Int)
(declare-const x142 Int)
(declare-const x143 Int)
(declare-const x144 Int)
(declare-const x145 Int)
(declare-const x146 Int)
(declare-const x147 Int)
(declare-const x148 Int)
(declare-const x149 Int)
(declare-const x150 Int)
(declare-const x151 Int)
(declare-const x152 Int)
(declare-const x153 Int)
(declare-const x154 Int)
(declare-const x155 Int)
(declare-const x156 Int)
(declare-const x157 Int)
(declare-const x158 Int)
(declare-const x159 Int)
(declare-const x160 Int)
(declare-const x161 Int)
(declare-const v160 Int)
(declare-const x162 Int)
(declare-const x163 Int)
(declare-const x164 Int)
(declare-const x165 Int)
(declare-const x166 Int)
(declare-const x167 Int)
(declare-const x168 Int)
(declare-const x169 Int)
(declare-const x170 Int)
(declare-const x171 Int)
(declare-const v170 Int)
(declare-const x172 Int)
(declare-const x173 Int)
(declare-const x174 Int)
(declare-const x175 Int)
(declare-const v174 Int)
(declare-const x176 Int)
(declare-const x177 Int)
(declare-const x178 Int)
(declare-const x179 Int)

(assert(and
(= x9 x92) (< x73 x35) (= x171 x24) (< x6 x86) 
(sep
(hls x0 x1 v0)
(hls x2 x3 v2)
(hls x4 x5 v4)
(blk x6 x7)
(blk x8 x9)
(pto x10 (c_data_t x11))
(blk x12 x13)
(pto x14 (c_data_t x15))
(blk x16 x17)
(pto x18 (c_data_t x19))
(blk x20 x21)
(hls x22 x23 v22)
(blk x24 x25)
(blk x26 x27)
(pto x28 (c_data_t x29))
(hls x30 x31 v30)
(blk x32 x33)
(blk x34 x35)
(hls x36 x37 v36)
(blk x38 x39)
(hls x40 x41 v40)
(hls x42 x43 v42)
(hls x44 x45 v44)
(pto x46 (c_data_t x47))
(pto x48 (c_data_t x49))
(blk x50 x51)
(hls x52 x53 v52)
(blk x54 x55)
(pto x56 (c_data_t x57))
(pto x58 (c_data_t x59))
(hls x60 x61 v60)
(hls x62 x63 v62)
(pto x64 (c_data_t x65))
(hls x66 x67 v66)
(pto x68 (c_data_t x69))
(hls x70 x71 v70)
(pto x72 (c_data_t x73))
(hls x74 x75 v74)
(hls x76 x77 v76)
(pto x78 (c_data_t x79))
(hls x80 x81 v80)
(hls x82 x83 v82)
(hls x84 x85 v84)
(hls x86 x87 v86)
(hls x88 x89 v88)
(pto x90 (c_data_t x91))
(pto x92 (c_data_t x93))
(blk x94 x95)
(blk x96 x97)
(hls x98 x99 v98)
(blk x100 x101)
(blk x102 x103)
(pto x104 (c_data_t x105))
(hls x106 x107 v106)
(pto x108 (c_data_t x109))
(pto x110 (c_data_t x111))
(blk x112 x113)
(hls x114 x115 v114)
(blk x116 x117)
(hls x118 x119 v118)
(hls x120 x121 v120)
(blk x122 x123)
(pto x124 (c_data_t x125))
(hls x126 x127 v126)
(hls x128 x129 v128)
(hls x130 x131 v130)
(blk x132 x133)
(blk x134 x135)
(blk x136 x137)
(blk x138 x139)
(blk x140 x141)
(blk x142 x143)
(blk x144 x145)
(blk x146 x147)
(blk x148 x149)
(blk x150 x151)
(pto x152 (c_data_t x153))
(blk x154 x155)
(pto x156 (c_data_t x157))
(pto x158 (c_data_t x159))
(hls x160 x161 v160)
(blk x162 x163)
(pto x164 (c_data_t x165))
(blk x166 x167)
(pto x168 (c_data_t x169))
(hls x170 x171 v170)
(pto x172 (c_data_t x173))
(hls x174 x175 v174)
(blk x176 x177)
(blk x178 x179)
)))

(check-sat)