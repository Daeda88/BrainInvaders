using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework;
using System.Xml;

namespace BrainInvadersLevelBuilder
{
    /**
     * Seperate block element of the Level Builder
     */
    class BIElement
    {
        BILevelBuilder levelBuilder;

        // We want to keep the path this element belongs to
        public BIPath path { get; set; }

        // We have some state.
        public BlockState currentState { get; private set; }
        int x, y;

        public BIElement(BILevelBuilder lb, int x, int y)
        {
            levelBuilder = lb;
            this.x = x;
            this.y = y;
            currentState = BlockState.EMPTY;
            path = null;
        }

        // States a block can have
        public enum BlockState {
            EMPTY,
            ALIEN1,
            ALIEN2,
            ALIEN3,
            TARGET,
            DISTRACTOR,
            DISTRACTOR2,
            END
        };

        public void Clicked()
        {
            // Update the state to the next one in the list.
            currentState = (BlockState)(((int)currentState + 1) % ((int)BlockState.END));
        }

        internal void writeXML(XmlTextWriter writer)
        {
            // If there is no path, we need to declare this alien as a seperate block.
            if (path == null)
            {
                writer.WriteStartElement("Block");

                writer.WriteStartElement("TotalAliens");
                writer.WriteString("1");
                writer.WriteEndElement();

                writer.WriteStartElement("Path");

                writer.WriteStartElement("NumOfSteps");
                writer.WriteString("1");
                writer.WriteEndElement();
                writer.WriteStartElement("Step");
                writer.WriteStartElement("X");
                writer.WriteString((x * 150).ToString());
                writer.WriteEndElement();
                writer.WriteStartElement("Y");
                writer.WriteString((y * 150).ToString());
                writer.WriteEndElement();
                writer.WriteEndElement();
                writer.WriteEndElement();
            }

            writer.WriteStartElement("Alien");

            writer.WriteStartElement("Name");
            writer.WriteString("A" + x + ":" + y);
            writer.WriteEndElement();

            writer.WriteStartElement("RelPos");

            writer.WriteStartElement("X");
            if (path == null)
                writer.WriteString("0");
            else
                writer.WriteString((x - path.lX).ToString());
            writer.WriteEndElement();
            writer.WriteStartElement("Y");
            if (path == null)
                writer.WriteString("0");
            else
                writer.WriteString((y - path.tY).ToString());
            writer.WriteEndElement();


            writer.WriteEndElement();

            writer.WriteStartElement("MatPos");
            writer.WriteStartElement("X");
            writer.WriteString(x.ToString());
            writer.WriteEndElement();
            writer.WriteStartElement("Y");
            writer.WriteString(y.ToString());
            writer.WriteEndElement();
            writer.WriteEndElement();

            writer.WriteStartElement("Type");
            writer.WriteString(((int)currentState - 1).ToString());
            writer.WriteEndElement();

            writer.WriteEndElement();

            // In case of seperate block, also close the block.
            if (path == null)
            {
                writer.WriteEndElement();
            }
        }
        
        public void Draw(SpriteBatch spriteBatch)
        {
            // Black square acting as a border
            spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(x * levelBuilder.blockWidth, y * levelBuilder.blockHeight, levelBuilder.blockWidth, levelBuilder.blockHeight), Color.Black);
            // Seperate alien logic
            if (currentState == (int)BlockState.EMPTY)
                spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.White);
            else
            {
                spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.Blue);

                switch (currentState)
                {
                    case BlockState.ALIEN1:
                        spriteBatch.Draw(levelBuilder.alienText1, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.White);
                        break;
                    case BlockState.ALIEN2:
                        spriteBatch.Draw(levelBuilder.alienText2, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.White);
                        break;
                    case BlockState.ALIEN3:
                        spriteBatch.Draw(levelBuilder.alienText3, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.White);
                        break;
                    case BlockState.TARGET:
                        spriteBatch.Draw(levelBuilder.alienTextT, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.Red);
                        break;
                    case BlockState.DISTRACTOR:
                        spriteBatch.Draw(levelBuilder.alienText1, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.Red);
                        break;
                    case BlockState.DISTRACTOR2:
                        spriteBatch.Draw(levelBuilder.alienText2, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.Green);
                        break;
                }

            }
        }

    }
}
