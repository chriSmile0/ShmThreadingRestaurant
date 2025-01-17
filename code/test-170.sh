#!/bin/sh

#
# Test de refoulement sur groupe absent
#

. ./ftest.sh

REPAS=300                               # durée d'un repas, en ms

debut_restaurant $REPAS 4 2 2           # 3 tables, pour ne pas bloquer C

# on occupe deux tables
duree_convive A 1 $((REPAS-MARGE)) $((REPAS+MARGE)) &
PID_CA=$!

msleep $MARGE

duree_convive B 1 $((REPAS-MARGE)) $((REPAS+MARGE)) &
PID_CB=$!

msleep $MARGE

# Monsieur X n'est pas encore arrivé
duree_convive C M.X 0 $MARGE &
PID_CC=$!

#police_puis_fermeture_restaurant

# attendre la fin du repas de A et B
msleep $REPAS

police_puis_fermeture_restaurant

fin_restaurant 2 2

# est-ce que tout le monde s'est bien terminé
wait $PID_CA || fail "A s'est mal terminé"
wait $PID_CB || fail "B s'est mal terminé"
wait $PID_CC || fail "C s'est mal terminé"

# normalement, le refoulement ne doit pas être vu comme une erreur
code_retour_convive_ok A "Code retour A invalide"
code_retour_convive_ok B "Code retour B invalide"
code_retour_convive_ok C "Refoulement de C != erreur"

logs_aux
echo "ok"
exit 0
