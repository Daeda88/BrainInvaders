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
     * Step in a BIPath
     */
    class PathNode
    {
        BILevelBuilder levelBuilder;
        PathNode next;

        public int x { get;  private set; }
        public int y { get;  private set; }

        public PathNode(BILevelBuilder lB, int x, int y)
        {
            this.x = x;
            this.y = y;
            levelBuilder = lB;
            next = null;
        }

        // Destroy node at end of the path
        public bool destroyNode(){
            // If this is not the end, call function on next
            if (next != null)
            {
                // Destroy reference to next if destroyed
                if (!next.destroyNode())
                    next = null;
                return true;
            }
            // If no next, delete this one
            else{
                return false;
            }
        }

        // Add a new step to the path
        public void addNode(int x, int y)
        {
            // If this is not the end, add to next
            if (next != null)
                next.addNode(x, y);
            // If last step, add to this
            else
                next = new PathNode(levelBuilder, x, y);
        }

        // Get the length of this path
        public int nodeLength()
        {
            // If the end, return 1
            if (next == null)
                return 1;
            // Else do some recursive stuff
            else
                return next.nodeLength() + 1;
        }

        // Write the XML
        public void writeXML(XmlTextWriter writer)
        {
            writer.WriteStartElement("Step");
            writer.WriteStartElement("X");
            writer.WriteString((x * 150).ToString());
            writer.WriteEndElement();
            writer.WriteStartElement("Y");
            writer.WriteString((y*-150).ToString());
            writer.WriteEndElement();
            writer.WriteEndElement();
            // If more steps, add them as well
            if (next != null)
                next.writeXML(writer);
        }

        public void Draw(SpriteBatch spriteBatch)
        {
            spriteBatch.Draw(levelBuilder.whiteTexture, new Rectangle(x * levelBuilder.blockWidth + 2, y * levelBuilder.blockHeight + 2, levelBuilder.blockWidth - 4, levelBuilder.blockHeight - 4), Color.Green);
            // If more steps, draw them too
            if (next != null)
                next.Draw(spriteBatch);
        }

    }
}
