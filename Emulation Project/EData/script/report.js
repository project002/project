$(function() {
	var dataset = [];
	var labels = [];
	$(".packetData").each(function() {
		var fil = parseInt($(this).find('.fillage').html(),10);
		var pid = parseInt($(this).find('.pID').html(),10);
		dataset.push(fil);
		labels.push(pid);
	});
	//add the chart data
	$("body").append("<h3>Fillage per packet</h3><canvas id='drC' width='1000' height='500'></canvas>");
	var data = {
		'labels':labels,
		'datasets':[
			{
				fillColor : "rgba(151,187,205,0.5)",
				strokeColor : "rgba(151,187,205,1)",
				pointColor : "rgba(151,187,205,1)",
				pointStrokeColor : "#fff",
				'data':dataset
			} 
	]};
	var ctx = $("#drC")[0].getContext("2d");
	var chart = new Chart(ctx).Line(data);
});