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
                    case 'te' : return 'At Time:';
                    default : return '';
            }
            return '';
	},
        getType : function(key)
        {
            switch(key)
            {
                    case 'pID': return '';
                    case 'pSize': return ' KB';
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
                var i = fromIndex;
                var count = 0;
                while (i<_data_len && count<length)
		{
                    if (ts[i].type === type) 
                    {
                        table.appendChild(row(count+fromIndex+1,ts[i])); //+1 for index to start from 1 not 0
                        count++;
                    }
                    ++i;
		}
		parent_.appendChild(table);
		
	}();
}

/**
* parent_ DOMEelement : where to output the graph to
* name String : the name of the graph
* X String : the tag for the X-axis of the graph
* Y String : the tag fot the Y-axis of the graph
* resolution int: how many sampling to take from the dataset (size of 'length' means all of them)
* fromIndex int: the start of the graph data in the data set
* length int : the length of the graph data in the data set
*/
var MakeGraph = function(parent_,name,X,Y,resolution,fromIndex,length)
{

	var _cid = 'gd';
	var _packetOffset = Utils.getPacketOffset();
	var _data_len = 0;
	var _chart = null;
	var _dataset = [];
	var _labels = [];


	var gatherData = function()
	{
		var sn = Math.floor(length / resolution);
		var end = fromIndex+length > _data_len ? _data_len : fromIndex+length;
		for (var i=(fromIndex+_packetOffset);i<end;i+=sn)
		{
			//n samples of the data
			// if ((i % sn)===0)
			// {
				_labels.push(ts[i][X]);
				_dataset.push(ts[i][Y]);
			// }
		}
		console.log(_dataset);
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
		_data_len = PDcount;
		_cid += X+Y;
		$(parent_).append("<h3>"+name+"</h3><canvas id='"+_cid+"' width='1000' height='500'></canvas>");
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
 */
var Range = function(parent,start,end,vformat,callback)
{
    var vis = {
      r : null, //range itself
      rs : null, //range start
      rsl: null, //range start label
      re : null, //range end 
      rel: null  //range end label
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
        
        parent.appendChild(p);
    };
    
    var end_drag = function()
    {
        drage = false;
        drags = false;
        callback(range_s,range_e);
    };
    
    var register_events = function()
    {
         $(vis.rs).mousedown(function() {drags=true;});
         $(vis.re).mousedown(function() {drage=true;});
         
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
                    console.log(range_s+" :: "+range_e);
                    if (range_e <= range_s) 
                    {
                        range_e = range_s+1;
                        v = drage ? range_e : (drags ? range_s : v);
                        return;
                    }
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

var makeSection  = function(name)
{
	var sec = document.createElement('section');
	sec.setAttribute('id','name_d');
	sec.setAttribute('class','sec name');
	var title = document.createElement('h2');
	title.innerHTML = name;
	sec.appendChild(title);
	return sec;
};

$(function() {
	// console.log(PDcount);
	// console.log(SDcount);
    ts.pop(); //remove the dummy element in the end
	var $$ = $('body');
    var run_time = Stats.getEmulationDuration();
	$$.append("<h1>Emulation Report:  ("+Utils.format_seconds(run_time)+")</h1>");
	
	var router_section = makeSection('Routers Data');
	DisplayData(router_section,'RD',0,10);
	$$.append(router_section);

	var stat_section = makeSection('Satistics Data');
        var temp  = function() {console.log('update')};
        Range(stat_section,0,run_time,Utils.format_seconds,temp);
	DisplayData(stat_section,'SD',0,ts.length);
        $$.append(stat_section);
        
        var overall_stat_section = makeSection('Overall Satistics');
	var t = document.createElement('table');
        var cols = ['gaf','gadr','asp'];
        for (var i in cols) {t.innerHTML += Stats.formatMean('SD',cols[i]);}
	overall_stat_section.appendChild(t);
        $$.append(overall_stat_section);
	

	//make the packets data visible
	var start = 0;
	var length = 5000;
	var packet_section = makeSection("Packet Data: (showing "+start+" - "+(start+length)+")");
	DisplayData(packet_section,'PD',start,length);
	$$.append(packet_section);	

	//make the graph section dynamic by properties
	var x_prop = 'pID'; //packetID
	var y_prop = 'dp'; //DropRate
	var graphs = makeSection("Graphs");	
	$$.append(graphs);
	MakeGraph(graphs,'Droprate/Packets',x_prop,y_prop,100,0,ts.length);
	MakeGraph(graphs,'Fillage/Packets',x_prop,'pSize',100,0,ts.length);

});