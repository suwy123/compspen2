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
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)
(declare-const x21 Int)
(declare-const x22 Int)
(declare-const x23 Int)
(declare-const x24 Int)
(declare-const x25 Int)
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
(declare-const x34 Int)
(declare-const x35 Int)
(declare-const x36 Int)
(declare-const x37 Int)
(declare-const x38 Int)
(declare-const x39 Int)
(declare-const v38 Int)
(declare-const x40 Int)
(declare-const x41 Int)
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
(declare-const v50 Int)
(declare-const x52 Int)
(declare-const x53 Int)
(declare-const x54 Int)
(declare-const x55 Int)
(declare-const x56 Int)
(declare-const x57 Int)
(declare-const v56 Int)
(declare-const x58 Int)
(declare-const x59 Int)
(declare-const x60 Int)
(declare-const x61 Int)
(declare-const x62 Int)
(declare-const x63 Int)
(declare-const x64 Int)
(declare-const x65 Int)
(declare-const x66 Int)
(declare-const x67 Int)
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
(declare-const x80 Int)
(declare-const x81 Int)
(declare-const x82 Int)
(declare-const x83 Int)
(declare-const x84 Int)
(declare-const x85 Int)
(declare-const v84 Int)
(declare-const x86 Int)
(declare-const x87 Int)
(declare-const x88 Int)
(declare-const x89 Int)
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
(declare-const x100 Int)
(declare-const x101 Int)
(declare-const x102 Int)
(declare-const x103 Int)
(declare-const v102 Int)
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
(declare-const v112 Int)
(declare-const x114 Int)
(declare-const x115 Int)
(declare-const x116 Int)
(declare-const x117 Int)
(declare-const v116 Int)
(declare-const x118 Int)
(declare-const x119 Int)
(declare-const x120 Int)
(declare-const x121 Int)
(declare-const x122 Int)
(declare-const x123 Int)
(declare-const v122 Int)
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
(declare-const x132 Int)
(declare-const x133 Int)
(declare-const x134 Int)
(declare-const x135 Int)
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
(declare-const v154 Int)
(declare-const x156 Int)
(declare-const x157 Int)
(declare-const x158 Int)
(declare-const x159 Int)

(assert(and
(< x73 x131) (< x34 x128) (distinct x143 x102) (< x63 x61) (distinct x109 x23) (= x27 x78) (distinct x8 x113) (= x49 x148) 
(sep
(pto x0 (c_data_t x1))
(hls x2 x3 v2)
(pto x4 (c_data_t x5))
(blk x6 x7)
(pto x8 (c_data_t x9))
(pto x10 (c_data_t x11))
(pto x12 (c_data_t x13))
(blk x14 x15)
(blk x16 x17)
(blk x18 x19)
(pto x20 (c_data_t x21))
(blk x22 x23)
(blk x24 x25)
(hls x26 x27 v26)
(hls x28 x29 v28)
(pto x30 (c_data_t x31))
(pto x32 (c_data_t x33))
(pto x34 (c_data_t x35))
(pto x36 (c_data_t x37))
(hls x38 x39 v38)
(pto x40 (c_data_t x41))
(hls x42 x43 v42)
(pto x44 (c_data_t x45))
(pto x46 (c_data_t x47))
(blk x48 x49)
(hls x50 x51 v50)
(blk x52 x53)
(pto x54 (c_data_t x55))
(hls x56 x57 v56)
(blk x58 x59)
(blk x60 x61)
(pto x62 (c_data_t x63))
(blk x64 x65)
(pto x66 (c_data_t x67))
(blk x68 x69)
(hls x70 x71 v70)
(blk x72 x73)
(hls x74 x75 v74)
(blk x76 x77)
(pto x78 (c_data_t x79))
(pto x80 (c_data_t x81))
(blk x82 x83)
(hls x84 x85 v84)
(pto x86 (c_data_t x87))
(pto x88 (c_data_t x89))
(pto x90 (c_data_t x91))
(pto x92 (c_data_t x93))
(blk x94 x95)
(pto x96 (c_data_t x97))
(pto x98 (c_data_t x99))
(pto x100 (c_data_t x101))
(hls x102 x103 v102)
(blk x104 x105)
(blk x106 x107)
(blk x108 x109)
(blk x110 x111)
(hls x112 x113 v112)
(pto x114 (c_data_t x115))
(hls x116 x117 v116)
(pto x118 (c_data_t x119))
(blk x120 x121)
(hls x122 x123 v122)
(pto x124 (c_data_t x125))
(hls x126 x127 v126)
(hls x128 x129 v128)
(blk x130 x131)
(pto x132 (c_data_t x133))
(pto x134 (c_data_t x135))
(blk x136 x137)
(hls x138 x139 v138)
(blk x140 x141)
(blk x142 x143)
(blk x144 x145)
(pto x146 (c_data_t x147))
(pto x148 (c_data_t x149))
(pto x150 (c_data_t x151))
(pto x152 (c_data_t x153))
(hls x154 x155 v154)
(pto x156 (c_data_t x157))
(pto x158 (c_data_t x159))
)))

(check-sat)