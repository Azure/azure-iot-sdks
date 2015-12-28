//  ------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation
//  All rights reserved. 
//  
//  Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this 
//  file except in compliance with the License. You may obtain a copy of the License at 
//  http://www.apache.org/licenses/LICENSE-2.0  
//  
//  THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR 
//  CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR 
//  NON-INFRINGEMENT. 
// 
//  See the Apache Version 2.0 License for specific language governing permissions and 
//  limitations under the License.
//  ------------------------------------------------------------------------------------

namespace Amqp
{
    interface INode
    {
        INode Next { get; set; }

        INode Previous { get; set; }
    }

    class LinkedList
    {
        INode head;
        INode tail;

        public INode First { get { return this.head; } }

        public void Add(INode node)
        {
            Fx.Assert(node.Previous == null && node.Next == null, "node is already in a list");
            if (this.head == null)
            {
                Fx.Assert(this.tail == null, "tail must be null");
                this.head = this.tail = node;
            }
            else
            {
                Fx.Assert(this.tail != null, "tail must not be null");
                this.tail.Next = node;
                node.Previous = this.tail;
                this.tail = node;
            }
        }

        public void Remove(INode node)
        {
            Fx.Assert(node != null, "node cannot be null");
            if (node == this.head)
            {
                this.head = node.Next;
                if (this.head == null)
                {
                    this.tail = null;
                }
                else
                {
                    this.head.Previous = null;
                }
            }
            else if (node == this.tail)
            {
                this.tail = node.Previous;
                this.tail.Next = null;
            }
            else if (node.Previous != null && node.Next != null)
            {
                // remove middle
                node.Previous.Next = node.Next;
                node.Next.Previous = node.Previous;
            }

            node.Previous = node.Next = null;
        }

        public INode Clear()
        {
            INode first = this.head;
            this.head = this.tail = null;
            return first;
        }
    }
}