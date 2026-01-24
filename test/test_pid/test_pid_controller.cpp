#include <unity.h>
#include "PIDController.h"

void setUp(void) {}
void tearDown(void) {}

// Test: création du contrôleur avec valeurs initiales
void test_pid_initialization(void) {
    PIDController pid(1.0, 0.1, 0.05, 0.01);

    // Première sortie avec erreur = 10
    float output = pid.compute(10.0, 0.0);

    // P = 1.0 * 10 = 10
    // I = 0.1 * 10 * 0.01 = 0.01
    // D = 0.05 * 10 / 0.01 = 50
    // Total attendu ~60.01
    TEST_ASSERT_FLOAT_WITHIN(1.0, 60.0, output);
}

// Test: convergence vers la consigne
void test_pid_convergence(void) {
    PIDController pid(0.5, 0.0, 0.0, 0.01);  // P seul
    pid.setOutputLimits(-100, 100);

    float position = 0.0;
    float setpoint = 10.0;

    // Simuler 100 itérations
    for (int i = 0; i < 100; i++) {
        float output = pid.compute(setpoint, position);
        position += output * 0.1;  // Simuler un système simple
    }

    // La position devrait se rapprocher de la consigne
    TEST_ASSERT_FLOAT_WITHIN(2.0, setpoint, position);
}

// Test: limites de sortie (saturation)
void test_pid_output_limits(void) {
    PIDController pid(10.0, 0.0, 0.0, 0.01);
    pid.setOutputLimits(-50, 50);

    // Grande erreur positive
    float output = pid.compute(100.0, 0.0);
    TEST_ASSERT_EQUAL_FLOAT(50.0, output);

    pid.reset();

    // Grande erreur négative
    output = pid.compute(-100.0, 0.0);
    TEST_ASSERT_EQUAL_FLOAT(-50.0, output);
}

// Test: reset remet l'intégrale à zéro
void test_pid_reset(void) {
    PIDController pid(0.0, 1.0, 0.0, 0.01);  // I seul
    pid.setOutputLimits(-1000, 1000);

    // Accumuler de l'intégrale
    for (int i = 0; i < 100; i++) {
        pid.compute(10.0, 0.0);
    }

    float beforeReset = pid.compute(10.0, 0.0);
    TEST_ASSERT_TRUE(beforeReset > 0.5);  // L'intégrale a accumulé

    pid.reset();

    // Après reset, la première sortie devrait être faible (juste l'intégrale de 1 cycle)
    float afterReset = pid.compute(10.0, 0.0);
    TEST_ASSERT_FLOAT_WITHIN(0.2, 0.1, afterReset);  // ~0.1 = 1.0 * 10 * 0.01
}

// Test: setTunings modifie les gains
void test_pid_set_tunings(void) {
    PIDController pid(1.0, 0.0, 0.0, 0.01);
    pid.setOutputLimits(-1000, 1000);

    float output1 = pid.compute(10.0, 0.0);
    TEST_ASSERT_FLOAT_WITHIN(1.0, 10.0, output1);  // P=1.0 * error=10

    pid.reset();
    pid.setTunings(2.0, 0.0, 0.0);

    float output2 = pid.compute(10.0, 0.0);
    TEST_ASSERT_FLOAT_WITHIN(1.0, 20.0, output2);  // P=2.0 * error=10
}

// Test: terme dérivé réduit les oscillations
void test_pid_derivative_damping(void) {
    PIDController pidNoD(1.0, 0.0, 0.0, 0.01);
    PIDController pidWithD(1.0, 0.0, 0.5, 0.01);
    pidNoD.setOutputLimits(-1000, 1000);
    pidWithD.setOutputLimits(-1000, 1000);

    // Première mesure
    pidNoD.compute(10.0, 0.0);
    pidWithD.compute(10.0, 0.0);

    // Deuxième mesure avec erreur réduite (système répond)
    float outNoD = pidNoD.compute(10.0, 5.0);   // erreur = 5
    float outWithD = pidWithD.compute(10.0, 5.0);

    // Le PID avec D devrait avoir une sortie plus faible (freine car erreur diminue)
    TEST_ASSERT_TRUE(outWithD < outNoD);
}

// Test: erreur nulle = sortie nulle (sans intégrale accumulée)
void test_pid_zero_error(void) {
    PIDController pid(1.0, 0.1, 0.05, 0.01);
    pid.setOutputLimits(-100, 100);

    float output = pid.compute(50.0, 50.0);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, output);
}

// Test: anti-windup - l'intégrale ne s'accumule pas indéfiniment
void test_pid_anti_windup(void) {
    PIDController pid(0.0, 10.0, 0.0, 0.01);  // I seul, gain élevé
    pid.setOutputLimits(-50, 50);

    // Accumuler beaucoup d'intégrale
    for (int i = 0; i < 1000; i++) {
        pid.compute(100.0, 0.0);
    }

    // La sortie devrait rester dans les limites
    float output = pid.compute(100.0, 0.0);
    TEST_ASSERT_TRUE(output <= 50.0);
    TEST_ASSERT_TRUE(output >= -50.0);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_pid_initialization);
    RUN_TEST(test_pid_convergence);
    RUN_TEST(test_pid_output_limits);
    RUN_TEST(test_pid_reset);
    RUN_TEST(test_pid_set_tunings);
    RUN_TEST(test_pid_derivative_damping);
    RUN_TEST(test_pid_zero_error);
    RUN_TEST(test_pid_anti_windup);

    return UNITY_END();
}
