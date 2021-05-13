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
(declare-const v14 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)

(assert(and
(distinct x4 x12) (distinct x1 x18) (< x8 x16) (= x1 x9) (< x10 x17) (= x7 x10) (< x7 x0) (< x3 x0) (distinct x17 x2)
(sep
(pto x0 (c_data_t x1))
(pto x2 (c_data_t x3))
(pto x4 (c_data_t x5))
(pto x6 (c_data_t x7))
(hls x8 x9 v8)
(blk x10 x11)
(hls x12 x13 v12)
(hls x14 x15 v14)
(blk x16 x17)
(pto x18 (c_data_t x19))
)))

(assert(not(and
(distinct x4 x12) (distinct x1 x18) (< x8 x16) (= x1 x9) (< x10 x17) (= x7 x10) (< x7 x0) (< x3 x0) (distinct x17 x2) 
(sep
(pto x0 (c_data_t x1))
(pto x2 (c_data_t x3))
(pto x4 (c_data_t x5))
(pto x6 (c_data_t x7))
(hls x8 x9 v8)
(blk x10 x11)
(hls x12 x13 v12)
(hls x14 x15 v14)
(blk x16 x17)
(pto x18 (c_data_t x19))
))))

(check-sat)