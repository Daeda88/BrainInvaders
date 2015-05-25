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
     * Describes a Block of BIElements and their movement.
     */
    class BIPath
    {

        BILevelBuilder levelBuilder;

        public List<BIElement> aliens { get; set; }
        public bool active { get; set; }

        public int lX { get; private set; }
        public int tY { get; private set; }
        int rX, bY;

        PathNode next;

        public BIPath(BILevelBuilder lb, int lX, int rX, int tY, int bY)
        {
            levelBuilder = lb;
            this.lX = lX;
            this.rX = rX;
            this.tY = tY;
            this.bY = bY;
            next = null;
            aliens = new List<BIElement>();
        }

        // Get the total amound of BIElements in this path
        private int AliensInBlock()
        {
            int result = 0;
            foreach (BIElement alien in aliens)
            {
                if (alien.currentState != BIElement.BlockState.EMPTY)
                    result++;
            }
            return result;
        }

        // Find the last node of the Path and delete it.
        public bool destroyNode()
        {
            // This is not the end of the path
            if (next != null)
            {
                // Make a nullpointer if next did a selfdestruct
                if (!next.destroyNode())
                    next = null;
                return true;
            }
            else
            {
                // Self destruct so reset path of all Aliens
                foreach (BIElement alien in aliens)
                {
                    alien.path = null;
                }
                return false;
            }
        }

        // Add a new step to the path
        public void addNode(int x, int y)
        {
            // Add to the next node if we have one
            if (next != null)
                next.addNode(x, y);
            // Else add to this one
            else
                next = new PathNode(levelBuilder, x, y);
        }

        public void writeXml(XmlTextWriter writer)
        {
            writer.WriteStartElement("Block");

            writer.WriteStartElement("TotalAliens");
            writer.WriteString(AliensInBlock().ToString());
            writer.WriteEndElement();

            writer.WriteStartElement("Path");

            writer.WriteStartElement("NumOfSteps");
            if (next != null)
                writer.WriteString((1 + next.nodeLength()).ToString());
            else
                writer.WriteString("1");
            writer.WriteEndElement();

            writer.WriteStartElement("Step");
            writer.WriteStartElement("X");
            writer.WriteString((lX * 150).ToString());
            writer.WriteEndElement();
            writer.WriteStartElement("Y");
            writer.WriteString((tY * -150).ToString());
            writer.WriteEndElement();
            writer.WriteEndElement();
            // Write XML for the next steps
            if (next != null)
                next.writeXML(writer);
            writer.WriteEndElement();

            // Write XML for all the Aliens in this block
            foreach (BIElement alien in aliens)
            {
                if (alien.currentState != 0)
                    alien.writeXML(writer);
            }

            writer.WriteEndElement();
        }

        public void Draw(SpriteBatch spriteBatch)
        {
            // Draw a border around the block
            spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(lX * levelBuilder.blockWidth, tY * levelBuilder.blockHeight, 3, (bY - tY +1) * levelBuilder.blockHeight), Color.Red);
            spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle((rX + 1) * levelBuilder.blockWidth - 3, tY * levelBuilder.blockHeight, 3, (bY - tY + 1) * levelBuilder.blockHeight), Color.Red);

            spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(lX * levelBuilder.blockWidth, tY * levelBuilder.blockHeight, (rX - lX + 1) * levelBuilder.blockWidth, 3), Color.Red);
            spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(lX * levelBuilder.blockWidth, (bY + 1) * levelBuilder.blockHeight - 3, (rX - lX + 1) * levelBuilder.blockWidth, 3), Color.Red);

            // If block selected, we show that
            if (active)
            {
                spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(lX * levelBuilder.blockWidth + 2, tY * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.Yellow);
                if (next != null)
                    next.Draw(spriteBatch);
            }
        }

    }
}
