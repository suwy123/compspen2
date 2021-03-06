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
(declare-const v6 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const v10 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const v12 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const v14 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const v16 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)
(declare-const x21 Int)
(declare-const v20 Int)
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
(declare-const v28 Int)
(declare-const x30 Int)
(declare-const x31 Int)
(declare-const x32 Int)
(declare-const x33 Int)
(declare-const v32 Int)
(declare-const x34 Int)
(declare-const x35 Int)
(declare-const v34 Int)
(declare-const x36 Int)
(declare-const x37 Int)
(declare-const v36 Int)
(declare-const x38 Int)
(declare-const x39 Int)
(declare-const v38 Int)
(declare-const x40 Int)
(declare-const x41 Int)
(declare-const v40 Int)
(declare-const x42 Int)
(declare-const x43 Int)
(declare-const v42 Int)
(declare-const x44 Int)
(declare-const x45 Int)
(declare-const x46 Int)
(declare-const x47 Int)
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
(declare-const x60 Int)
(declare-const x61 Int)
(declare-const x62 Int)
(declare-const x63 Int)
(declare-const v62 Int)
(declare-const x64 Int)
(declare-const x65 Int)
(declare-const x66 Int)
(declare-const x67 Int)
(declare-const x68 Int)
(declare-const x69 Int)
(declare-const x70 Int)
(declare-const x71 Int)
(declare-const x72 Int)
(declare-const x73 Int)
(declare-const v72 Int)
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
(declare-const x82 Int)
(declare-const x83 Int)
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
(declare-const x94 Int)
(declare-const x95 Int)
(declare-const x96 Int)
(declare-const x97 Int)
(declare-const x98 Int)
(declare-const x99 Int)
(declare-const x100 Int)
(declare-const x101 Int)
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
(declare-const v120 Int)
(declare-const x122 Int)
(declare-const x123 Int)
(declare-const x124 Int)
(declare-const x125 Int)
(declare-const x126 Int)
(declare-const x127 Int)
(declare-const x128 Int)
(declare-const x129 Int)
(declare-const x130 Int)
(declare-const x131 Int)
(declare-const x132 Int)
(declare-const x133 Int)
(declare-const v132 Int)
(declare-const x134 Int)
(declare-const x135 Int)
(declare-const v134 Int)
(declare-const x136 Int)
(declare-const x137 Int)
(declare-const x138 Int)
(declare-const x139 Int)
(declare-const v138 Int)
(declare-const x140 Int)
(declare-const x141 Int)
(declare-const x142 Int)
(declare-const x143 Int)
(declare-const x144 Int)
(declare-const x145 Int)
(declare-const v144 Int)
(declare-const x146 Int)
(declare-const x147 Int)
(declare-const x148 Int)
(declare-const x149 Int)

(assert(and
(distinct x146 x52) (< x67 x92) (= x2 x149) (= x120 x44) (distinct x121 x113) (distinct x7 x50) (= x59 x34) (= x95 x136) 
(sep
(hls x0 x1 v0)
(hls x2 x3 v2)
(hls x4 x5 v4)
(hls x6 x7 v6)
(pto x8 (c_data_t x9))
(hls x10 x11 v10)
(hls x12 x13 v12)
(hls x14 x15 v14)
(hls x16 x17 v16)
(blk x18 x19)
(hls x20 x21 v20)
(hls x22 x23 v22)
(hls x24 x25 v24)
(hls x26 x27 v26)
(hls x28 x29 v28)
(pto x30 (c_data_t x31))
(hls x32 x33 v32)
(hls x34 x35 v34)
(hls x36 x37 v36)
(hls x38 x39 v38)
(hls x40 x41 v40)
(hls x42 x43 v42)
(pto x44 (c_data_t x45))
(blk x46 x47)
(pto x48 (c_data_t x49))
(pto x50 (c_data_t x51))
(blk x52 x53)
(pto x54 (c_data_t x55))
(pto x56 (c_data_t x57))
(blk x58 x59)
(blk x60 x61)
(hls x62 x63 v62)
(pto x64 (c_data_t x65))
(blk x66 x67)
(pto x68 (c_data_t x69))
(pto x70 (c_data_t x71))
(hls x72 x73 v72)
(hls x74 x75 v74)
(hls x76 x77 v76)
(blk x78 x79)
(pto x80 (c_data_t x81))
(blk x82 x83)
(hls x84 x85 v84)
(hls x86 x87 v86)
(blk x88 x89)
(hls x90 x91 v90)
(blk x92 x93)
(pto x94 (c_data_t x95))
(blk x96 x97)
(pto x98 (c_data_t x99))
(blk x100 x101)
(blk x102 x103)
(blk x104 x105)
(blk x106 x107)
(pto x108 (c_data_t x109))
(pto x110 (c_data_t x111))
(blk x112 x113)
(hls x114 x115 v114)
(hls x116 x117 v116)
(hls x118 x119 v118)
(hls x120 x121 v120)
(pto x122 (c_data_t x123))
(pto x124 (c_data_t x125))
(pto x126 (c_data_t x127))
(blk x128 x129)
(pto x130 (c_data_t x131))
(hls x132 x133 v132)
(hls x134 x135 v134)
(pto x136 (c_data_t x137))
(hls x138 x139 v138)
(pto x140 (c_data_t x141))
(pto x142 (c_data_t x143))
(hls x144 x145 v144)
(blk x146 x147)
(blk x148 x149)
)))

(check-sat)