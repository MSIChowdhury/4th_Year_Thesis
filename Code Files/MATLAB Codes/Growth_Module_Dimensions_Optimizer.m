% Define variables
W = 0;
L = 0;
H = 15;
t = 0.125;
D = 6;
% Define base_area and material_area
base_area = (W-2*t)*(L-2*t);
material_area = 2*(H*L + H*W) + L*W;
% Initialize arrays to store the values
result_data = [];
unique_pairs = [];
% Iterate over W and L
index = 1;
for W = 12:0.125:100
    for L = 12:0.125:100
        % Check if (W-2t)/D and (L-2t)/D are integers
        if mod(W-2*t,D) == 0 && mod(L-2*t,D) == 0
            % Calculate and store |base_area - material_area|
            result = abs(base_area - material_area);
            result_data = [result_data; W, L, result];
            % Store unique pairs of L and W
            if isempty(unique_pairs) || ~any(ismember(unique_pairs, [L, W], 'rows'))
                unique_pairs = [unique_pairs; L, W];
            end
        end
    end
end
% Write the results to a CSV file
writematrix(result_data, 'results.csv');
% Print the unique pairs of L and W
disp('Unique Pairs of L and W:');
disp(unique_pairs);