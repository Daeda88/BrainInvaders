using System;

namespace BrainInvadersLevelBuilder
{
#if WINDOWS || XBOX
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            using (BILevelBuilder game = new BILevelBuilder())
            {
                game.Run();
            }
        }
    }
#endif
}

