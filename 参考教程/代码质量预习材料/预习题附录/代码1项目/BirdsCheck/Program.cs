﻿using System;

namespace BirdsCheck
{
    class Program
    {
        //主函数
        static void Main(string[] args)
        {
            Bird x = new Bird();
            penguin a = new penguin();
            eagle b = new eagle();
            chicken c = new chicken();
            if(a.CanFly == x.CanFly)
            {
                Console.WriteLine("penguin is bird");
            }
            else
            {
                Console.WriteLine("penguin is not bird");
            }
            if (b.CanFly == x.CanFly)
            {
                Console.WriteLine("eagle is bird");
            }
            else
            {
                Console.WriteLine("eagle is not bird");
            }
            if (c.CanFly == x.CanFly)
            {
                Console.WriteLine("chicken is bird");
            }
            else
            {
                Console.WriteLine("chicken is not bird");
            }
            Console.ReadKey();
        }
    }

    //鸟
    class Bird
    {
        public bool CanFly = true;
    }

    //企鹅
    class penguin
    {
        public bool CanFly = false;
    }

    //老鹰
    class eagle
    {
        public bool CanFly = true;
    }

    //鸡
    class chicken
    {
        public bool CanFly = false;
    }
}
