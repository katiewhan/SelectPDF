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

    }
}
