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
(declare-const v4 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)

(assert(and
(distinct x8 x4) (= x3 x0) (< x7 x0) (distinct x0 x2) (< x4 x9)
(= x4 (+ x2 1)) (= (+ x2 x3) x10) (< x4 x10) (< x10 x5)
(< x0 x2) (< x5 x6) (< x6 x8)
(sep
(pto x0 (c_data_t x1))
(pto x2 (c_data_t x3))
(hls x4 x5 v4)
(pto x6 (c_data_t x7))
(blk x8 x9)
)))

(assert(not(and
(distinct x8 x4) (= x3 x0) (< x7 x0) (distinct x0 x2) (< x4 x9) 
(sep
(pto x0 (c_data_t x1))
(hls x2 x10 x3)
(blk x10 (+ x5 1))
(pto x6 (c_data_t x7))
(blk x8 x9)
))))

(check-sat)