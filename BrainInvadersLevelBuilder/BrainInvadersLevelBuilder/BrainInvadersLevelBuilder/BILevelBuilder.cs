using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;
using System.Xml;
using System.IO;
using Microsoft.Xna.Framework.Storage;

namespace BrainInvadersLevelBuilder
{
    /// <summary>
    /// This program helps to quickly build a level for Brain Invaders.
    /// Build with the XNA framework. See www.xna.com to get all the framework libraries.
    /// Beginners to the framework are recommended to read the following tutorial: http://www.riemers.net/eng/Tutorials/XNA/Csharp/series2d.php
    /// </summary>
    public class BILevelBuilder : Microsoft.Xna.Framework.Game
    {
        static int MAXBLOCKSX = 9;
        static int MAXBLOCKSY = 6;

        public int blockWidth;
        public int blockHeight;

        // Some variables needed for drawing
        GraphicsDeviceManager graphics;
        GraphicsDevice device;
        SpriteBatch spriteBatch;

        // Some input logic.
        MouseState prevMouseState;
        bool dragging;

        // Internal game logic variables.
        BIElement[][] blockMatrix;
        List<BIPath> pathList;
        bool pathSelected;
        BIPath selectedPath;

        // Graphic elements
       // public Texture2D alien1Texture;
        public Texture2D whiteTexture;

        public Texture2D alienText1, alienText2, alienText3, alienTextT;


        public BILevelBuilder()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            // Lets set our window to the required settings
            device = graphics.GraphicsDevice;
            graphics.PreferredBackBufferHeight = 480;
            graphics.PreferredBackBufferWidth = 854;
            IsMouseVisible = true;
            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);

            // Calculate the size of the blocks
            blockWidth = ((graphics.PreferredBackBufferWidth - 50) / MAXBLOCKSX);
            blockHeight = ((graphics.PreferredBackBufferHeight - 50) / MAXBLOCKSY);

            // Fill the on-screen matrix with elements
            blockMatrix = new BIElement[MAXBLOCKSX][];
            for (int i = 0; i < MAXBLOCKSX; i++)
            {
                blockMatrix[i] = new BIElement[MAXBLOCKSY];
                for (int j = 0; j < MAXBLOCKSY; j++)
                {
                    blockMatrix[i][j] = new BIElement(this, i, j);
                }
            }

            pathList = new List<BIPath>();

            //alien1Texture = Content.Load<Texture2D>("Alien_1_1");

            // Create a small single coloured texture.
            Color[] whiteTextCol = new Color[1];
            whiteTextCol[0] = Color.White;
            whiteTexture = new Texture2D(device, 1, 1, false, SurfaceFormat.Color);
            whiteTexture.SetData(whiteTextCol);

            alienText1 = Content.Load<Texture2D>("Alien_1_1");
            alienText2 = Content.Load<Texture2D>("Alien_2_1");
            alienText3 = Content.Load<Texture2D>("Alien_3_1");
            alienTextT = Content.Load<Texture2D>("Alien_T");
        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content here
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // Allows the game to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();

            // Handle the mouse.
            MouseState currentMouseState = Mouse.GetState();
            dragging = false;
            // Left mouse button released
            if (prevMouseState.LeftButton == ButtonState.Pressed && currentMouseState.LeftButton == ButtonState.Released)
            {
                // Mouse hasnt moved, to its a click
                if (Math.Abs(prevMouseState.X - currentMouseState.X) < 3 && Math.Abs(prevMouseState.Y - currentMouseState.Y) < 3)
                {
                    int mouseX = currentMouseState.X / blockWidth;
                    int mouseY = currentMouseState.Y / blockHeight;
                    // If released above a block, lets click said block
                    if (mouseX < MAXBLOCKSX && mouseY < MAXBLOCKSY)
                    {
                        
                        // When we are simply clicking a block, change its type, if we are clicking paths, add new direction.
                        if (!pathSelected)
                        {
                            blockMatrix[mouseX][mouseY].Clicked();
                        }
                        else
                        {
                            selectedPath.addNode(mouseX, mouseY);
                        }
                    }
                        // If out of bouds we want to construct the level.
                    else
                    {
                        MakeXML();
                    }
                }
                // If we are dragging, lets select the field dragged over.
                else if (!pathSelected)
                {
                    int mouseXL = prevMouseState.X / blockWidth;
                    int mouseYT = prevMouseState.Y / blockHeight;

                    int mouseXR = currentMouseState.X / blockWidth;
                    int mouseYB = currentMouseState.Y / blockHeight;

                    if (mouseXL > mouseXR)
                    {
                        int tempX = mouseXR;
                        mouseXR = mouseXL;
                        mouseXL = tempX;
                    }
                    if (mouseYT > mouseYB)
                    {
                        int tempY = mouseYT;
                        mouseYT = mouseYB;
                        mouseYB = tempY;
                    }
                    if (mouseYB >= MAXBLOCKSY)
                        mouseYB = MAXBLOCKSY - 1;
                    if (mouseXR >= MAXBLOCKSX)
                        mouseXR = MAXBLOCKSX - 1;
                    List<BIElement> blockElements = new List<BIElement>();
                    BIPath path = new BIPath(this, mouseXL, mouseXR, mouseYT, mouseYB);
                    bool canMakeBlock = true;
                    // Get all the blocks in the selected field
                    for (int i = mouseXL; i <= mouseXR; i++)
                        for (int j = mouseYT; j <= mouseYB; j++)
                        {
                            canMakeBlock = canMakeBlock && blockMatrix[i][j].path == null;
                            blockElements.Add(blockMatrix[i][j]);
                        }
                    // If selected block doesnt overlap another block, make it.
                    if (canMakeBlock)
                    {
                        pathList.Add(path);
                        path.aliens = blockElements;
                        for (int i = mouseXL; i <= mouseXR; i++)
                            for (int j = mouseYT; j <= mouseYB; j++)
                            {
                                blockMatrix[i][j].path = path;
                            }
                    }
                }
                    
            }
            // Right mouse results in Path selection
            if (prevMouseState.RightButton == ButtonState.Pressed && currentMouseState.RightButton == ButtonState.Released && Math.Abs(prevMouseState.X - currentMouseState.X) < 3 && Math.Abs(prevMouseState.Y - currentMouseState.Y) < 3)
            {
                if (pathSelected)
                    selectedPath.active = false;
                pathSelected = false;
                // Get corresponding path
                int mouseX = currentMouseState.X / blockWidth;
                int mouseY = currentMouseState.Y / blockHeight;
                selectedPath = blockMatrix[mouseX][mouseY].path;
                if (selectedPath != null)
                {
                    pathSelected = true;
                    selectedPath.active = true;
                }
            }
            // Middlebutton will destroy a node of the selected path (if selected)
            if (pathSelected && prevMouseState.MiddleButton == ButtonState.Pressed && currentMouseState.MiddleButton == ButtonState.Released)
            {
                if (!selectedPath.destroyNode())
                {
                    
                    pathList.Remove(selectedPath);
                    selectedPath = null;
                    pathSelected = false;
                }
            }
            // If we are not dragging, remember the current mouse state.
            if (!(prevMouseState.LeftButton == ButtonState.Pressed && currentMouseState.LeftButton == ButtonState.Pressed))
                prevMouseState = currentMouseState;
            else
                dragging = true;
            
            base.Update(gameTime);
        }

        /**
         * Create the XML file of this game.
         */
        private void MakeXML()
        {
            // We need to be able to create a document. Due to the nature of XNA (taking an Xbox into account) this gets a bit complicated
            // Files are saved in //Users/<Name>/Documents/Saved Games/BrainInvadersLevelBuilder/StorageDemo/Player1/

            // We need to asynchronously ask for the saving directory. Should be immediate on a PC.
            IAsyncResult result = StorageDevice.BeginShowSelector(PlayerIndex.One, null, null);
            result.AsyncWaitHandle.WaitOne();
            StorageDevice storageDevice = StorageDevice.EndShowSelector(result);
            result.AsyncWaitHandle.Close();

            result = storageDevice.BeginOpenContainer("StorageDemo", null, null);
            result.AsyncWaitHandle.WaitOne();
            StorageContainer container = storageDevice.EndOpenContainer(result);

            // Close the wait handle.
            result.AsyncWaitHandle.Close();

            // With saving directory available, get the file to save in.
            string filename = "level.xml";
            if (container.FileExists(filename))
                container.DeleteFile(filename);
            Stream file = container.OpenFile(filename, FileMode.OpenOrCreate);

            // Make an XmlWriter and parse through everything, using BI-level-logic.
            XmlTextWriter writer = new XmlTextWriter(file, System.Text.Encoding.UTF8);

            writer.WriteStartDocument();

            writer.WriteStartElement("Level");

            writer.WriteStartElement("TotalBlocks");
            writer.WriteString(pathList.Count.ToString());
            writer.WriteEndElement();

            writer.WriteStartElement("StepSpeed");
            writer.WriteString("2.5");
            writer.WriteEndElement();

            writer.WriteStartElement("SpeedIncrease");
            writer.WriteString("0.925");
            writer.WriteEndElement();

            writer.WriteStartElement("MaxSpeed");
            writer.WriteString("1.0");
            writer.WriteEndElement();

            // Lets create a seperate block for each path
            foreach (BIPath path in pathList)
                path.writeXml(writer);

            // Any aliens that might not be added to a path will be set to an invidual block.
            for (int i = 0; i < MAXBLOCKSX; i++)
                for (int j = 0; j < MAXBLOCKSY; j++)
                    if (blockMatrix[i][j].path == null && blockMatrix[i][j].currentState != 0)
                        blockMatrix[i][j].writeXML(writer);
            writer.WriteEndElement();

            writer.WriteEndDocument();
            writer.Close();

            // Dispose the container.
            container.Dispose();

            // Close the game as we are done.
            Exit();

        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            // Clear the screen to a Blue Color.
            GraphicsDevice.Clear(Color.CornflowerBlue);

            // Begin drawing the sprites
            spriteBatch.Begin();
            // Draw each block
            for (int i = 0; i < MAXBLOCKSX; i++)
            {
                for (int j = 0; j < MAXBLOCKSY; j++)
                {
                    blockMatrix[i][j].Draw(spriteBatch);
                }
            }

            // Draw each path
            foreach (BIPath path in pathList)
            {
                path.Draw(spriteBatch);
            }

            // Stop the sprite drawing for this frame.
            spriteBatch.End();
            base.Draw(gameTime);
        }
    }
}
