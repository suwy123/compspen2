(set-logic QF_SHIDLIA)
(set-info :source | Songbird - https://songbird-prover.github.io/ |)
(set-info :smt-lib-version 2)
(set-info :category "crafted")
(set-info :status unsat)

;; singleton heap

(declare-sort Refnode 0)

(declare-datatypes
 ((node 0))
 (((c_node (next Refnode) (prev Refnode)))))

(declare-heap (Refnode node))

;; heap predicates

(define-fun-rec dll ((hd_2 Refnode) (p_3 Refnode) (tl_4 Refnode) (n_5 Refnode) (len_6 Int)) Bool
  (or
   (and
    (pto hd_2 (c_node n_5 p_3))
    (and
     (= (+ len_6 (- 1)) 0)
     (= hd_2 tl_4)))
   (exists
    ((x_7 Refnode) (k Int))
    (and
     (sep
      (pto hd_2 (c_node x_7 p_3))
      (dll x_7 hd_2 tl_4 n_5 k))
     (= k (+ len_6 (- 1)))
     (<= 1 (+ len_6 (- 1)))))))

;; heap predicates

(define-fun-rec ls_pre ((x_8 Refnode) (y_9 Refnode) (len_10 Int)) Bool
  (or
   (and
    (_ emp Refnode node)
    (and
     (= len_10 0)
     (= x_8 y_9)))
   (exists
    ((anon_11 Refnode) (u_12 Refnode) (k Int))
    (and
     (sep
      (pto x_8 (c_node anon_11 u_12))
      (ls_pre u_12 y_9 k))
     (= k (+ len_10 (- 1)))
     (<= 0 (+ len_10 (- 1)))))))

(check-sat)

;; entailment: dll(x,y,z,t,n) & 100<=n |- ls_pre(z,y,n)

(declare-const x Refnode)
(declare-const y Refnode)
(declare-const z Refnode)
(declare-const t Refnode)
(declare-const n Int)

(assert
 (and
  (dll x y z t n)
  (<= 100 n)))

(assert
 (not
  (ls_pre z y n)))

(check-sat)
