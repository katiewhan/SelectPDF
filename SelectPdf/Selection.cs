using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SelectPdf
{
    class Selection
    {
        private int id { get; set; }
        private List<MuRect> highlights = new List<MuRect>();
        private List<MuSelection> contents = new List<MuSelection>();
        private int pageNum { get; set; }

        public Selection(int id, int page)
        {
            this.id = id;
            this.pageNum = page;
        }

        public void AddHighlightRect(MuRect rect)
        {
            highlights.Add(rect);
        }

        public List<MuRect> GetHighlightRects()
        {
            return highlights;
        }

        public bool HasContents()
        {
            return contents.Count > 0;
        }

        public void AddContent(MuSelection content)
        {
            contents.Add(content);
        }

        public List<MuSelection> GetContents()
        {
            return contents;
        }
    }
}
