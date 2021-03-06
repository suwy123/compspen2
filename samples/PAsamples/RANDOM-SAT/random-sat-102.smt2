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
(declare-const v16 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)
(declare-const x21 Int)
(declare-const x22 Int)
(declare-const x23 Int)
(declare-const v22 Int)
(declare-const x24 Int)
(declare-const x25 Int)
(declare-const v24 Int)
(declare-const x26 Int)
(declare-const x27 Int)
(declare-const v26 Int)
(declare-const x28 Int)
(declare-const x29 Int)
(declare-const x30 Int)
(declare-const x31 Int)
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
(declare-const x42 Int)
(declare-const x43 Int)
(declare-const v42 Int)
(declare-const x44 Int)
(declare-const x45 Int)
(declare-const x46 Int)
(declare-const x47 Int)
(declare-const v46 Int)
(declare-const x48 Int)
(declare-const x49 Int)
(declare-const x50 Int)
(declare-const x51 Int)
(declare-const x52 Int)
(declare-const x53 Int)
(declare-const x54 Int)
(declare-const x55 Int)
(declare-const x56 Int)
(declare-const x57 Int)
(declare-const x58 Int)
(declare-const x59 Int)
(declare-const v58 Int)
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
(declare-const x78 Int)
(declare-const x79 Int)
(declare-const v78 Int)
(declare-const x80 Int)
(declare-const x81 Int)
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
(declare-const x90 Int)
(declare-const x91 Int)
(declare-const v90 Int)
(declare-const x92 Int)
(declare-const x93 Int)
(declare-const v92 Int)
(declare-const x94 Int)
(declare-const x95 Int)
(declare-const v94 Int)
(declare-const x96 Int)
(declare-const x97 Int)
(declare-const x98 Int)
(declare-const x99 Int)
(declare-const x100 Int)
(declare-const x101 Int)
(declare-const v100 Int)
(declare-const x102 Int)
(declare-const x103 Int)
(declare-const x104 Int)
(declare-const x105 Int)
(declare-const x106 Int)
(declare-const x107 Int)
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
(declare-const v116 Int)
(declare-const x118 Int)
(declare-const x119 Int)
(declare-const v118 Int)
(declare-const x120 Int)
(declare-const x121 Int)
(declare-const x122 Int)
(declare-const x123 Int)
(declare-const x124 Int)
(declare-const x125 Int)
(declare-const v124 Int)
(declare-const x126 Int)
(declare-const x127 Int)
(declare-const x128 Int)
(declare-const x129 Int)
(declare-const x130 Int)
(declare-const x131 Int)
(declare-const x132 Int)
(declare-const x133 Int)
(declare-const x134 Int)
(declare-const x135 Int)
(declare-const v134 Int)
(declare-const x136 Int)
(declare-const x137 Int)
(declare-const x138 Int)
(declare-const x139 Int)
(declare-const x140 Int)
(declare-const x141 Int)
(declare-const x142 Int)
(declare-const x143 Int)
(declare-const v142 Int)
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
(declare-const v152 Int)
(declare-const x154 Int)
(declare-const x155 Int)
(declare-const x156 Int)
(declare-const x157 Int)
(declare-const x158 Int)
(declare-const x159 Int)
(declare-const x160 Int)
(declare-const x161 Int)
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
(declare-const v178 Int)
(declare-const x180 Int)
(declare-const x181 Int)
(declare-const x182 Int)
(declare-const x183 Int)
(declare-const x184 Int)
(declare-const x185 Int)
(declare-const x186 Int)
(declare-const x187 Int)
(declare-const x188 Int)
(declare-const x189 Int)
(declare-const v188 Int)
(declare-const x190 Int)
(declare-const x191 Int)
(declare-const x192 Int)
(declare-const x193 Int)
(declare-const v192 Int)
(declare-const x194 Int)
(declare-const x195 Int)
(declare-const x196 Int)
(declare-const x197 Int)
(declare-const v196 Int)
(declare-const x198 Int)
(declare-const x199 Int)

(assert(and
(distinct x169 x95) (< x148 x15) (distinct x141 x83) (distinct x112 x195) 
(sep
(hls x0 x1 v0)
(hls x2 x3 v2)
(pto x4 (c_data_t x5))
(blk x6 x7)
(pto x8 (c_data_t x9))
(blk x10 x11)
(blk x12 x13)
(pto x14 (c_data_t x15))
(hls x16 x17 v16)
(pto x18 (c_data_t x19))
(blk x20 x21)
(hls x22 x23 v22)
(hls x24 x25 v24)
(hls x26 x27 v26)
(pto x28 (c_data_t x29))
(pto x30 (c_data_t x31))
(blk x32 x33)
(blk x34 x35)
(hls x36 x37 v36)
(blk x38 x39)
(pto x40 (c_data_t x41))
(hls x42 x43 v42)
(blk x44 x45)
(hls x46 x47 v46)
(pto x48 (c_data_t x49))
(pto x50 (c_data_t x51))
(blk x52 x53)
(blk x54 x55)
(blk x56 x57)
(hls x58 x59 v58)
(hls x60 x61 v60)
(hls x62 x63 v62)
(pto x64 (c_data_t x65))
(hls x66 x67 v66)
(pto x68 (c_data_t x69))
(hls x70 x71 v70)
(blk x72 x73)
(hls x74 x75 v74)
(blk x76 x77)
(hls x78 x79 v78)
(blk x80 x81)
(hls x82 x83 v82)
(hls x84 x85 v84)
(hls x86 x87 v86)
(blk x88 x89)
(hls x90 x91 v90)
(hls x92 x93 v92)
(hls x94 x95 v94)
(blk x96 x97)
(pto x98 (c_data_t x99))
(hls x100 x101 v100)
(pto x102 (c_data_t x103))
(blk x104 x105)
(blk x106 x107)
(blk x108 x109)
(blk x110 x111)
(blk x112 x113)
(hls x114 x115 v114)
(hls x116 x117 v116)
(hls x118 x119 v118)
(blk x120 x121)
(pto x122 (c_data_t x123))
(hls x124 x125 v124)
(pto x126 (c_data_t x127))
(blk x128 x129)
(blk x130 x131)
(pto x132 (c_data_t x133))
(hls x134 x135 v134)
(blk x136 x137)
(blk x138 x139)
(blk x140 x141)
(hls x142 x143 v142)
(blk x144 x145)
(pto x146 (c_data_t x147))
(pto x148 (c_data_t x149))
(pto x150 (c_data_t x151))
(hls x152 x153 v152)
(pto x154 (c_data_t x155))
(pto x156 (c_data_t x157))
(blk x158 x159)
(blk x160 x161)
(pto x162 (c_data_t x163))
(pto x164 (c_data_t x165))
(pto x166 (c_data_t x167))
(blk x168 x169)
(hls x170 x171 v170)
(pto x172 (c_data_t x173))
(hls x174 x175 v174)
(pto x176 (c_data_t x177))
(hls x178 x179 v178)
(pto x180 (c_data_t x181))
(pto x182 (c_data_t x183))
(pto x184 (c_data_t x185))
(pto x186 (c_data_t x187))
(hls x188 x189 v188)
(blk x190 x191)
(hls x192 x193 v192)
(pto x194 (c_data_t x195))
(hls x196 x197 v196)
(pto x198 (c_data_t x199))
)))

(check-sat)