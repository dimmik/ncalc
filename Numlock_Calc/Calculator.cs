using System;

namespace Numlock_Calc
{
    public static class Calculator
    {
        public static double Add(double a, double b) => a + b;

        public static double Subtract(double a, double b) => a - b;

        public static double Multiply(double a, double b) => a * b;

        public static double Divide(double a, double b)
        {
            if (b == 0)
            {
                throw new DivideByZeroException("Cannot divide by zero.");
            }
            return a / b;
        }

        public static double SquareRoot(double a)
        {
            if (a < 0)
            {
                throw new ArgumentException("Cannot calculate the square root of a negative number.");
            }
            return Math.Sqrt(a);
        }

        public static double Square(double a) => a * a;

        public static double Power(double @base, double exponent) => Math.Pow(@base, exponent);

        public static double Log(double a, double baseLog = 10)
        {
            if (a <= 0)
            {
                throw new ArgumentException("Logarithm is only defined for positive numbers.");
            }
            return Math.Log(a, baseLog);
        }
    }
}
