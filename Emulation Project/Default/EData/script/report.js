var Utils = {
	/*Get the  readable name of a dataset key*/
	getName : function(key)
	{
            switch(key)
            {
                    case 'pID': return 'packet ID';
                    case 'pSize': return 'packet Size';
                    case 'rID': return 'Router ID';
                    case 'insT': return 'Insert Time';
                    case 'exitT': return 'Exit Time';
                    case 'totalT': return 'Total Time';
                    case 'fil': return 'Fillage';
                    case 'dp': return 'Drop Rate';
                    case 'ue': return 'Total Time Until Exit';
                    case 'bsz': return 'Buffer Size';
                    case 'bus': return 'Buffer Used Size';
                    case 'gaf' : return 'Average Fillage';
                    case 'gadr': return 'Average DropRate';
                    case 'asp' : return 'Average Speed';
                    case 'te' : return 'Time';
                    default : return '';
            }
            return '';
	},
        getType : function(key)
        {
            switch(key)
            {
                    case 'pID': return '';
                    case 'pSize': return ' B';
                    case 'rID': return '';
                    case 'insT': return 'ms';
                    case 'exitT': return 'ms';
                    case 'totalT': return 'ms';
                    case 'fil': return '%';
                    case 'dp': return '%';
                    case 'ue': return 'ms';
                    case 'bsz': return 'p';
                    case 'bus': return '%';
                    case 'gaf' : return '%';
                    case 'gadr': return '%';
                    case 'asp' : return ' KB/s';
                    case 'te' : return 's';
                    default : return '';
            }
            return '';
        },
	getPacketOffset : function()
	{
		for (var i=0;i<ts.length;++i)
		{
			if (ts[i].type !== 'RD') 
			{
				return i;
			}
		}
		return ts.length;
	},
    getDataOffset : function(type,startIndex)
    {
        var count = 0;
        var reverse = false;
        if (type==='PD') {reverse = ((100/PDcount)*startIndex) > 50;}
        if (type==='SD') {reverse = ((100/SDcount)*startIndex) > 50;}
        var i = reverse ? ts.length-1 : 0;
        while (count<=startIndex) 
        {
            if (ts[i].type === type) {++count;}
            if (reverse) {--i;} else {++i;}
            if (reverse && i<0) {break;}
            if (!reverse && i>=ts.length) {break;}
        }
        return i-1;
    },
    //format seconds to hr:min:sec
    format_seconds : function(seconds)
    {
        var t = [seconds % 60,
                 Math.floor(seconds / 60) % 60,
                 Math.floor(seconds / 3600)];
        for (var i in t) {t[i] = t[i] < 10 ? "0"+t[i] : t[i];}
        return t[2]+":"+t[1]+":"+t[0];
    }
};

var Stats = {
  /*
   * gets the emulation duration by the last packet to exit it
   * return the time in seconds , zero else
   * */  
  getEmulationDuration : function()
  {
      var len = ts.length;
      for (var i=len-1;i>=0;--i)
      {
          if (ts[i].type === 'PD') {return Math.round(ts[i].exitT/1000);}
      }
      return 0;
  },
  mean : function(type,col)
  {
      var len = ts.length;
      var sum = 0;
      var count = 0;
      for (var i=len-1;i>=0;--i)
      {
          if (ts[i].type === type) 
          {
              sum += ts[i][col];
              count++;
          }
      }
      if (count < 1) {return 0;}
      return (sum/count).toFixed(3);
  },
  formatMean : function(type,col)
  {
      var label = "<td class='lblmean'>"+Utils.getName(col)+"</td>";
      return "<tr>"+label+"<td class='spmean'>"+Stats.mean(type,col)+Utils.getType(col)+"</td></tr>";
  }
};

/**
* parent_ DOMEelement : where to output the data to
* type String : determins if it's router Data ('RD') or packet Data ('PD')
* fromIndex int: the start of the graph data in the data set
* length int : the length of the graph data in the data set
*/
var DisplayData = function(parent_,type,fromIndex,length)
{
	var _packetDataOffset = 0; //holds the packet data offset in the data object
	var _data_len = 0; //the length of the data set
	


	var row = function(index,data)
	{
		var r = document.createElement('tr');
		r.setAttribute('class','row');
                var id = document.createElement('td');
                id.innerHTML = index;
                id.setAttribute('class','index');
                r.appendChild(id);
		for (var t in data)
		{
			if (t !== 'type')
			{
				var l = document.createElement('td');
				l.innerHTML = Utils.getName(t);
				l.setAttribute('class','label');
				var d = document.createElement('td');
				d.setAttribute('class',t);
				d.innerHTML = data[t]+Utils.getType(t);
				r.appendChild(l);
				r.appendChild(d);
			}
		}
		return r;
	}

	var init = function()
	{
		if (typeof ts === 'undefined') {return;}
		if (fromIndex < 0) {return;}
		if (length < 1) {return;}
		_data_len = ts.length;
		//calculate the packet data offset
		_packetDataOffset = Utils.getPacketOffset();
		var table = document.createElement('table');
		// table.setAttribute('border','1');
		//if (type==='PD') {fromIndex+=_packetDataOffset;}
		//make sure it length doesn't go out of bounds
//		var end = fromIndex+length > _data_len ? _data_len : fromIndex+length;
//		for (var i=fromIndex;i<end;++i)
                var i = Utils.getDataOffset(type,fromIndex);
                var count = 0;
                while (i<_data_len && count<length)
		{
                    if (window.ts[i].type === type) 
                    {
                        table.appendChild(row(count+fromIndex+1,window.ts[i])); //+1 for index to start from 1 not 0
                        count++;
                    }
                    ++i;
		}
                $(parent_).find('table').remove();
		parent_.appendChild(table);
		
	}();
}

/**
* parent_ DOMEelement : where to output the graph to
* name String : the name of the graph
* X String : the tag for the X-axis of the graph
* Y String : the tag fot the Y-axis of the graph
* resolution int: how many samples to take from the dataset (size of 'length' means all of them)
* fromIndex int: the start of the graph data in the data set
* length int : the length of the graph data in the data set
*/
var MakeGraph = function(parent_,name,X,Y,resolution,fromIndex,length,type)
{

	var _cid = 'gd';
	var _packetOffset = Utils.getPacketOffset();
	var _data_len = 0;
	var _chart = null;
	var _dataset = [];
	var _labels = [];
        var _type = (typeof type === 'undefined') ? 'PD' : type;

	var gatherData = function()
	{
		var sn = Math.floor(length / resolution);
		var end = fromIndex+length > _data_len ? _data_len : fromIndex+length;
		for (var i=(fromIndex+_packetOffset);i<end;i+=sn)
		{
			//n samples of the data
			 if (ts[i].type === _type)
			 {
				_labels.push(ts[i][X]);
				_dataset.push(ts[i][Y]);
			 }
		
		}
//		 console.log("L:"+_labels);
//		 console.log("D:"+_dataset);
	}

	var getGraphDataset = function()
	{
		var dataset = {
			'labels':_labels,
			'datasets':[
				{
					fillColor : "rgba(151,187,205,0.5)",
					strokeColor : "rgba(151,187,205,1)",
					pointColor : "rgba(151,187,205,1)",
					pointStrokeColor : "#fff",
					'data':_dataset
				} 
		]};
		return dataset;
	}

	var init = function()
	{
		if (resolution < 1) {return;}
		if (fromIndex < 0) {return;}
		if (length < 1) {return;}
		if (Utils.getName(X) === '') {return;}
		if (Utils.getName(Y) === '') {return;}
		if (resolution > length) {resolution = length;}
		_data_len = PDcount;
		_cid += X+Y;
		$(parent_).append("<h3>"+name+"</h3><canvas id='"+_cid+"' width='1400' height='500'></canvas>");
		var ctx = $("#"+_cid)[0].getContext("2d");
		_chart = new Chart(ctx);
		gatherData();
		_chart.Line(getGraphDataset());
	}();
}


/**
 * makes a multiple range selector
 * @param DOMElmenent parent the parent DOM element
 * @param int start the start value 
 * @param int end the end value
 * @param function vformat function that formats the start & end values visually
 * @param function callback the value change callback get called with the new values
 * gets the percentages of the range (ps,pe)
 */
var Range = function(parent,start,end,vformat,callback)
{
    var vis = {
      r : null, //range itself
      rs : null, //range start
      rsl: null, //range start label
      re : null, //range end 
      rel: null,  //range end label
      btn: null //button to activate the select
    };
    var drage = false;
    var drags = false;
    var range_s = 0;
    var range_e = 0;
    var len = 0;
    
    var add_visuals = function()
    {
        var p = document.createElement('div');
        p.setAttribute('class','range-sel');
        
        vis.r = document.createElement('div');
        vis.r.setAttribute('class','range-line');
        vis.r.innerHTML = '&nbsp;';
        p.appendChild(vis.r);
        
        var st = document.createElement('label');
        st.setAttribute('class','range-start');
        st.innerHTML = vformat(start);
        p.appendChild(st);
        
        var en = document.createElement('label');
        en.setAttribute('class','range-end');
        en.innerHTML = vformat(end);
        p.appendChild(en);
        
        vis.rs = document.createElement('div');
        vis.rs.setAttribute('class','range-rs');
        p.appendChild(vis.rs);
        
        vis.rsl = document.createElement('label');
        vis.rsl.setAttribute('class','range-rsl');
        vis.rsl.innerHTML = vformat(range_s);
        vis.rs.appendChild(vis.rsl);
        
        vis.re = document.createElement('div');
        vis.re.setAttribute('class','range-re');
        p.appendChild(vis.re);
        
        vis.rel = document.createElement('label');
        vis.rel.setAttribute('class','range-rel');
        vis.rel.innerHTML = vformat(range_e);
        vis.re.appendChild(vis.rel);
        
        
        vis.btn = document.createElement('button');
        vis.btn.setAttribute('class','range-btn');
        vis.btn.innerHTML = "Show Selection";
        p.appendChild(vis.btn);
        
        parent.appendChild(p);
    };
    
    var end_drag = function()
    {
        drage = false;
        drags = false;
    };
    
    var select_range = function()
    {
        if (len < 1) {return;}
        var ps = (100/len)*range_s;
        var pe = (100/len)*range_e;
        callback.apply(RangeCallbacks,[ps,pe]);
    };
    
    var register_events = function()
    {
        $(vis.rs).mousedown(function() {drags=true;});
        $(vis.re).mousedown(function() {drage=true;});
        $(vis.btn).click(select_range);
         
        $(window).mouseup(end_drag);
        $(window).mousemove(function(e) {
            var drag = drags || drage;
            var $$ = drage ? $(vis.re) : (drags ? $(vis.rs) : null);
            var $$l = drage ? $(vis.rel) : (drags ? $(vis.rsl) : null);
            if ($$===null) {return;}
            if (drag && e.pageX)
            {
                var mk = (parseInt($$.width(),10)/2);
                var kwid = $$.width();
                var range = parseInt($(vis.r).width(),10);
                var pixel_offset = (e.pageX - parseInt($(vis.r).offset().left,10))+mk;
                pixel_offset = ((pixel_offset+kwid) > $(vis.r).width()) ? $(vis.r).width()+mk : pixel_offset;
                pixel_offset = ((pixel_offset-kwid) < 0) ? mk : pixel_offset;
                var right = (-pixel_offset+parseInt($(vis.r).width(),10));
                if (right < 0) {right = 0;}
                if (right > range) {right = range;}
                if (range > 0)
                {
                    var v = len-Math.floor((len/range)*right);
                    if (drags) {range_s = v;}
                    if (drage) {range_e = v;}
                    if (range_e <= range_s) 
                    {
                        if (drage) {range_e = range_s+1;}
                        if (drags) {range_s = range_e-1;}
                        v = drage ? range_e : (drags ? range_s : v);
                        return;
                    }
                    // console.log(range_s+" :: "+range_e);
                    $$l.html(vformat(v));
                }
                $$.css('right',right+'px');
            }
        });
    };
    
    var init = function()
    {
        if (end < start) {return;}
        if (typeof vformat !== 'function') {return;}
        if (typeof callback !== 'function') {return;}
        range_s = start;
        range_e = end;
        len = end-start;
        add_visuals();
        register_events();
    }();
};

var makeSection  = function(name,exportb)
{
	var sec = document.createElement('section');
	sec.setAttribute('class','sec');
	var title = document.createElement('h2');
        title.setAttribute('class','sec-title');
	title.innerHTML = name;
	sec.appendChild(title);
        if (typeof exportb === 'string') {CSVexport(sec,exportb);}
        var content = document.createElement('div');
        content.setAttribute('class','sec-content');
        sec.appendChild(content);
        $("body").append(sec);
	return content;
};

/**
* pElem DomElement the parent element
* start int the start of the input selection
* end int the end of the input selection
* type string the type key of the data
**/
var GraphCreate = function(pElem,start,end,type)
{
	var range = null;
	var prop_x_sel = null;
	var prop_y_sel = null;
	var samplesNum = null;
	var keys = [];
	var localParent = null;
	var graphView = null;

	var update_sel = function(ps,pe)
	{
            var start = Math.floor((PDcount/100)*ps);
            var end = Math.ceil((PDcount/100)*pe);
            var count = end-start;


            var xprop = prop_x_sel.options[prop_x_sel.selectedIndex].value;
            var yprop = prop_y_sel.options[prop_y_sel.selectedIndex].value;
            var samp  = parseInt(samplesNum.value,10);
            // console.log(start+","+(end-start));
            graphView.innerHTML = "";
            MakeGraph(graphView,Utils.getName(xprop)+"/"+Utils.getName(yprop),xprop,yprop,samp,start,end-start,type);
	}

	var label = function(str,forElem) 
	{
		var l = document.createElement('label');
		l.innerHTML = str;
		var r = "s"+Math.random();
		l.setAttribute('for',r);
		forElem.setAttribute('id',r);
		return l;
	}

	var select_prop = function()
	{
		var s = document.createElement('select');
                
		for (var i=0;i<keys.length;++i)
		{
                    if (keys[i] === 'type') {continue;}
                    var o = document.createElement('option');
                    o.value = keys[i];
                    o.innerHTML = Utils.getName(keys[i]);
                    s.appendChild(o);
		}
		return s;
	}

	var samples_con = function()
	{
		var ins = document.createElement('input');
		ins.setAttribute('type','text');
		ins.setAttribute('class','g_sam');
		ins.setAttribute('value',50);
		return ins;
	}

	var numeric_only = function(e)
	{
		var key = e.keyCode || e.which;
		switch (key)
		{
			case 8: //backspace
			case 46: //delete
			case 39: //arrows right left
			case 37: return true;
		}
		key = String.fromCharCode( key );
		var regex = /[0-9]|\./;
		if( !regex.test(key) ) 
		{
			e.preventDefault();
			return false;
  		}
  		return true;
	}

	var init = function()
	{
		if (typeof pElem === 'undefined') {return;}
		//gather the keys from an actual row in the dataset
		var pdOffset = Utils.getDataOffset(type,0);
		var example_row = window.ts[pdOffset];
		for (var key in example_row) {keys.push(key);} 
		
		localParent = document.createElement('div');
		localParent.setAttribute('class','graph_sel');

		graphView = document.createElement('div');
		graphView.setAttribute('class','graph_view');

		//local parent fot the controls

		prop_x_sel = select_prop();
		psxl = label('X:',prop_x_sel);
		prop_y_sel = select_prop();
		psyl = label("Y:",prop_y_sel);
		samplesNum = samples_con();
		saml = label("Samples:",samplesNum);
		$(samplesNum).keypress(numeric_only);

		localParent.appendChild(psxl);
		localParent.appendChild(prop_x_sel);
		localParent.appendChild(psyl);
		localParent.appendChild(prop_y_sel);
		localParent.appendChild(saml);
		localParent.appendChild(samplesNum);

		range = new Range(localParent,start,end,Utils.format_seconds,update_sel);	

		pElem.appendChild(localParent);

		//local parent to the graph view

		pElem.appendChild(graphView);

	}();
};

var CSVexport = function(container,type)
{
    var _sep = "";
    var trow = function(i)
    {
        var row = "";
        for(var k in ts[i])
        {
            if (k === 'type') {continue;}
            row += _sep+ts[i][k];
            _sep = ",";
        }
        return row;
    };
    
    var thead = function(i)
    {
        var row = "";
        for(var k in ts[i])
        {
            if (k === 'type') {continue;}
            row += _sep+Utils.getName(k);
            _sep = ",";
        }
        return row;
    };
    
    var exportCSV = function()
    {
        var w=window.open('text/plain','CSV',
                    'width=450,height=250'
                     +',menubar=0'
                     +',toolbar=1'
                     +',status=0'
                     +',scrollbars=1'
                     +',resizable=1');
        var offset = Utils.getDataOffset(type,0);
        w.document.writeln(thead(offset));
        var count = type==='PD' ? PDcount : 0;
        count = type==='SD' ? SDcount : count;
        var counter = 0;
        var i = offset;
        while(counter<count)
        {
            if (ts[i].type === type)
            {
                ++counter;
                w.document.writeln(trow(i));
            }
            ++i;
        }
        w.document.close();
    };
    
    var init = function()
    {
        var btn = document.createElement('button');
        btn.setAttribute('class','csv_e');
        btn.innerHTML = "Export to CSV";
        $(btn).click(exportCSV);
        container.appendChild(btn);
    }();
}

var RangeCallbacks = {
  SDparent : null,
  PDparent : null,
  countWarn : 5000,
  range : function(ps,pe,len)
  {
    var start = Math.floor((len/100)*ps);
    var end = Math.ceil((len/100)*pe);
    var count = end-start;
    if (count > RangeCallbacks.countWarn) 
    {
        var a =confirm("This will show more than 10,000 (~"+count+")entries, are you sure?");
        if (!a) {return false;}
    }
    return [start,end-start];
  },
  SDrange : function(ps,pe)
  {
    var ret = RangeCallbacks.range(ps,pe,SDcount);
    if (ret===false) {return;}
    {DisplayData(RangeCallbacks.SDparent,'SD',ret[0],ret[1]);}
  },
  PDrange : function(ps,pe)
  {
    var ret = RangeCallbacks.range(ps,pe,PDcount);
    if (ret===false) {return;}
    {DisplayData(RangeCallbacks.PDparent,'PD',ret[0],ret[1]);}
  }
};

$(function() {
//    console.log(PDcount);
//    console.log(SDcount);
    ts.pop(); //remove the dummy element in the end
    var $$ = $('body');
    var run_time = Stats.getEmulationDuration();
    $$.append("<h1>Emulation Report:  ("+Utils.format_seconds(run_time)+")</h1>");

    var router_section = makeSection('Routers Data');
    DisplayData(router_section,'RD',0,10);

    var stat_section = makeSection('Satistics Data',"SD");
    var start = 0;
    var end = 10;
    RangeCallbacks.SDparent = stat_section;
    Range(stat_section,0,run_time,Utils.format_seconds,RangeCallbacks.SDrange);
    DisplayData(stat_section,'SD',start,end-start);

    var overall_stat_section = makeSection('Overall Satistics');
    var t = document.createElement('table');
    var cols = ['gaf','gadr','asp'];
    for (var i in cols) {t.innerHTML += Stats.formatMean('SD',cols[i]);}
    overall_stat_section.appendChild(t);


    //make the packets data visible
    start = 0;
    end = 50;
    var packet_section = makeSection("Packet Data:","PD");
    RangeCallbacks.PDparent = packet_section;
    Range(packet_section,0,run_time,Utils.format_seconds,RangeCallbacks.PDrange);
    DisplayData(packet_section,'PD',start,end-start);

    var satGraph = makeSection("Satistics Graphs");
    GraphCreate(satGraph,0,run_time,'SD');

    //make the graph section dynamic by properties
    var x_prop = 'pID'; //packetID
    var y_prop = 'dp'; //DropRate
    var graphs = makeSection("Custom Graphs");	
    GraphCreate(graphs,0,run_time,'PD');
    
    //simple toggle
    $(".sec-content").hide();
    $(".sec-title").css('cursor','pointer');
    $(".sec-title").click(function() {$(this.parentNode).find('.sec-content').toggle();});
    
});