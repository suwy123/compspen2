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
(declare-const v8 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const v12 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)

(assert(and
(< x8 x16) (distinct x13 x1) (= x16 x19) (distinct x13 x7) (= x0 x5) 
(sep
(blk x0 x1)
(hls x2 x3 v2)
(pto x4 (c_data_t x5))
(pto x6 (c_data_t x7))
(hls x8 x9 v8)
(pto x10 (c_data_t x11))
(hls x12 x13 v12)
(blk x14 x15)
(pto x16 (c_data_t x17))
(blk x18 x19)
)))

(assert(not(and
(= x13 x1)
(sep
(blk x0 x1)
(hls x2 x3 v2)
(pto x4 (c_data_t x5))
(pto x6 (c_data_t x7))
(hls x8 x9 v8)
(pto x10 (c_data_t x11))
(hls x12 x13 v12)
(blk x14 x15)
(pto x16 (c_data_t x17))
(blk x18 x19)
))))

(check-sat)